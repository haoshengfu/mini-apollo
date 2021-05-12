#ifndef MINIAPOLLO_ENVIRONMENT_H
#define MINIAPOLLO_ENVIRONMENT_H

#include <assert.h>
#include <string>

#include "log.h"

namespace apollo {
    namespace cyber {
        namespace common {

            inline const std::string GetEnv(const std::string& var_name) {
                char* var = nullptr;
                var = getenv(var_name.c_str());
                if (var == nullptr) {
                    AWARN << "Environment variable ["<< var_name << "] not set";
                    return std::string("");
                }
                return std::string(var);
            }

            inline const std::string WorkRoot() {
                std::string work_root = GetEnv("CYBER_PATH");
                if (work_root.empty()) {
                    work_root = "/Users/seaside/Desktop/mini-apollo";
                }
                return work_root;
            }

        }  // namespace common
    }  // namespace cyber
}  // namespace apollo

#endif //MINIAPOLLO_ENVIRONMENT_H
