//
// Created by seaside on 2021/4/20.
//

#include "module_controller.h"

#include <utility>

#include "../common/environment.h"
#include "../common/file.h"
#include "../component/component_base.h"

namespace apollo {
    namespace cyber {
        namespace mainboard {

            ModuleController::ModuleController(const ModuleArgument& args) { args_ = args; }

            ModuleController::~ModuleController() {}

            bool ModuleController::Init() { return LoadAll(); }

            void ModuleController::Clear() {
               for (auto& component : component_list_) {
                   component->Shutdown();
               }
                component_list_.clear();  // keep alive
               class_loader_manager_.UnloadAllLibrary();
            }

            bool ModuleController::LoadAll() {
                const std::string work_root = common::WorkRoot();
                const std::string current_path = common::GetCurrentPath();
                const std::string dag_root_path = common::GetAbsolutePath(work_root, "dag");
                //找到模块的路径
                for (auto& dag_conf : args_.GetDAGConfList()) {
                    std::string module_path = "";
                    if (dag_conf == common::GetFileName(dag_conf)) {
                        // case dag conf argument var is a filename
                        module_path = common::GetAbsolutePath(dag_root_path, dag_conf);
                    } else if (dag_conf[0] == '/') {
                        // case dag conf argument var is an absolute path
                        module_path = dag_conf;
                    } else {
                        // case dag conf argument var is a relative path
                        module_path = common::GetAbsolutePath(current_path, dag_conf);
                        if (!common::PathExists(module_path)) {
                            module_path = common::GetAbsolutePath(work_root, dag_conf);
                        }
                    }
                    if (!LoadModule(module_path)) {
                        AERROR << "Failed to load module: " << module_path;
                        return false;
                    }
                }
                return true;
            }

            bool ModuleController::LoadModule(const DagConfig& dag_config) {
                const std::string work_root = common::WorkRoot();

                for (auto module_config : dag_config.module_config()) {
                    std::string load_path;
                    if (module_config.module_library().front() == '/') {
                        load_path = module_config.module_library();
                    } else {
                        load_path =
                                common::GetAbsolutePath(work_root, module_config.module_library());
                    }
                    if (!common::PathExists(load_path)) {
                        AERROR << "Path not exist: " << load_path;
                        return false;
                    }
                    //1. 加载动态库libcommon_component_example.so
                    class_loader_manager_.LoadLibrary(load_path);
                    // 2. 根据配置信息初始化各个component实例
                   for (auto& component : module_config.components()) {
                       const std::string& class_name = component.class_name();
                       std::shared_ptr<ComponentBase> base =
                               class_loader_manager_.CreateClassObj<ComponentBase>(class_name);
                       if (base == nullptr) {
                           return false;
                       }

                       if (!base->Initialize(component.config())) {
                           return false;
                       }
                       component_list_.emplace_back(std::move(base));
                   }

                   for (auto& component : module_config.timer_components()) {
                       const std::string& class_name = component.class_name();
                       std::shared_ptr<ComponentBase> base =
                               class_loader_manager_.CreateClassObj<ComponentBase>(class_name);
                       if (base == nullptr) {
                           return false;
                       }

                       if (!base->Initialize(component.config())) {
                           return false;
                       }
                       component_list_.emplace_back(std::move(base));
                   }
                }
                return true;
            }

            bool ModuleController::LoadModule(const std::string& path) {
                DagConfig dag_config;
                if (!common::GetProtoFromFile(path, &dag_config)) {
                    AERROR << "Get proto failed, file: " << path;
                    return false;
                }
                return LoadModule(dag_config);
            }

        }  // namespace mainboard
    }  // namespace cyber
}  // namespace apollo
