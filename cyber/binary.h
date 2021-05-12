//
// Created by seaside on 2021/4/20.
//

#ifndef MINIAPOLLO_BINARY_H
#define MINIAPOLLO_BINARY_H
#include <string>
namespace apollo {
    namespace cyber {
        class Binary {
        public:
            static std::string GetName() { return GetNameRef(); }
            static void SetName(const std::string& name) { GetNameRef() = name; }
            static std::string& GetNameRef() {
                static std::string binary_name;
                return binary_name;
            }
        };
    }  // namespace cyber
}  // namespace apollo
#endif //MINIAPOLLO_BINARY_H
