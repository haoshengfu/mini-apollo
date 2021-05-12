//
// Created by seaside on 2021/4/20.
//

#ifndef MINIAPOLLO_MODULE_ARGUMENTS_H
#define MINIAPOLLO_MODULE_ARGUMENTS_H


#include <list>
#include <string>

#include "../common/global_data.h"
#include "../common/log.h"
#include "../common/types.h"

namespace apollo {
    namespace cyber {
        namespace mainboard {

            static const char DEFAULT_process_group_[] = "mainboard_default";
            static const char DEFAULT_sched_name_[] = "CYBER_DEFAULT";

            class ModuleArgument {
            public:
                ModuleArgument() = default;
                virtual ~ModuleArgument() = default;
                void ParseArgument(int argc, char* const argv[]);
                void GetOptions(const int argc, char* const argv[]);
                inline std::string GetBinaryName() const { return binary_name_; }
                inline std::string GetProcessGroup() const { return process_group_; }
                inline std::string GetSchedName() const { return sched_name_; }
                inline std::list<std::string> GetDAGConfList() const {
                    return dag_conf_list_;
                }

            private:
                std::list<std::string> dag_conf_list_;
                std::string binary_name_;
                std::string process_group_;
                std::string sched_name_;
            };

        }  // namespace mainboard
    }  // namespace cyber
}  // namespace apollo

#endif //MINIAPOLLO_MODULE_ARGUMENTS_H
