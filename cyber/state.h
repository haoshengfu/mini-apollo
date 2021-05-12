#ifndef MINIAPOLLO_STATE_H
#define MINIAPOLLO_STATE_H

#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <stdint.h>
#include <cstring>
#include <thread>
#include <signal.h>

#include "common/log.h"

namespace apollo {
    namespace cyber {

        enum State {
            STATE_UNINITIALIZED = 0,
            STATE_INITIALIZED,
            STATE_SHUTTING_DOWN,
            STATE_SHUTDOWN,
        };

        State GetState();
        void SetState(const State& state);

        inline bool OK() { return GetState() == STATE_INITIALIZED; }

        inline bool IsShutdown() {
            return GetState() == STATE_SHUTTING_DOWN || GetState() == STATE_SHUTDOWN;
        }

         inline void WaitForShutdown() {
             while (!IsShutdown()) {
                 std::this_thread::sleep_for(std::chrono::milliseconds(200));
             }
         }

         inline void AsyncShutdown() {
            pid_t pid = getpid();
            if (kill(pid, SIGINT) != 0) {
                AERROR << strerror(errno);
            }
         }

    }  // namespace cyber
}  // namespace apollo

//
#endif //MINIAPOLLO_STATE_H
