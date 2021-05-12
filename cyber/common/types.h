//
// Created by seaside on 2021/4/20.
//

#ifndef MINIAPOLLO_TYPES_H
#define MINIAPOLLO_TYPES_H

#include <cstdint>

namespace apollo {
    namespace cyber {

        class NullType {};

// Return code definition for cyber internal function return.
        enum ReturnCode {
            SUCC = 0,
            FAIL = 1,
        };

/**
 * @brief Describe relation between nodes, writers/readers...
 */
        enum Relation : std::uint8_t {
            NO_RELATION = 0,
            DIFF_HOST,  // different host
            DIFF_PROC,  // same host, but different process
            SAME_PROC,  // same process
        };

        static const char SRV_CHANNEL_REQ_SUFFIX[] = "__SRV__REQUEST";
        static const char SRV_CHANNEL_RES_SUFFIX[] = "__SRV__RESPONSE";

    }  // namespace cyber
}  // namespace apollo


#endif //MINIAPOLLO_TYPES_H
