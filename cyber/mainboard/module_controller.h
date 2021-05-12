//
// Created by seaside on 2021/4/20.
//

#ifndef MINIAPOLLO_MODULE_CONTROLLER_H
#define MINIAPOLLO_MODULE_CONTROLLER_H

#include <memory>
#include <string>
#include <vector>

#include "../class_loader/class_loader_manager.h"
#include "../component/component.h"
#include "module_argument.h"
#include "../proto/dag_conf.pb.h"

namespace apollo {
    namespace cyber {
        namespace mainboard {

            using apollo::cyber::proto::DagConfig;

            class ModuleController {
            public:
                explicit ModuleController(const ModuleArgument& args);
                virtual ~ModuleController();

                bool Init();
                bool LoadAll();
                void Clear();

            private:
                bool LoadModule(const std::string& path);
                bool LoadModule(const DagConfig& dag_config);

                ModuleArgument args_;
                class_loader::ClassLoaderManager class_loader_manager_;
                std::vector<std::shared_ptr<ComponentBase>> component_list_;
            };

        }  // namespace mainboard
    }  // namespace cyber
}  // namespace apollo


#endif //MINIAPOLLO_MODULE_CONTROLLER_H
