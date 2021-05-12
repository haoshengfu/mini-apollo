//
// Created by seaside on 2021/4/20.
//

#ifndef MINIAPOLLO_INIT_H
#define MINIAPOLLO_INIT_H


#include "common/log.h"
#include "state.h"

namespace apollo {
    namespace cyber {

        bool Init(const char* binary_name);
        void Clear();

    }  // namespace cyber
}  // namespace apollo

#endif //MINIAPOLLO_INIT_H
