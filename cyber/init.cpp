//
// Created by seaside on 2021/4/20.
//

#include "init.h"

#include <libgen.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <csignal>

#include "binary.h"
#include "common/environment.h"
#include "common/file.h"
#include "common/global_data.h"

#include "data/data_dispatcher.h"
#include "event/perf_event_cache.h"
#include "logger/async_logger.h"
#include "scheduler/scheduler.h"
#include "task/task.h"
#include "timer/timer_manager.h"
#include "transport/transport.h"
#include "scheduler/scheduler_factory.h"

namespace apollo {
    namespace cyber {

        using apollo::cyber::common::EnsureDirectory;
        using apollo::cyber::common::GetAbsolutePath;
        using apollo::cyber::common::GetProtoFromFile;
        using apollo::cyber::common::WorkRoot;
       using apollo::cyber::croutine::CRoutine;
       using apollo::cyber::event::PerfEventCache;
       using apollo::cyber::scheduler::Scheduler;

        namespace {
            bool g_atexit_registered = false;
            std::mutex g_mutex;
            logger::AsyncLogger* async_logger = nullptr;
        }

        namespace {

            void InitLogger(const char* binary_name) {
                const char* slash = strrchr(binary_name, '/');
                if (slash) {
                    ::apollo::cyber::Binary::SetName(slash + 1);
                } else {
                    ::apollo::cyber::Binary::SetName(binary_name);
                }
                CHECK_NOTNULL(common::GlobalData::Instance());

                // Init glog
                google::InitGoogleLogging(binary_name);
                google::SetLogDestination(google::ERROR, "");
                google::SetLogDestination(google::WARNING, "");
                google::SetLogDestination(google::FATAL, "");

                // Init async logger
               async_logger = new ::apollo::cyber::logger::AsyncLogger(
                       google::base::GetLogger(FLAGS_minloglevel), 2 * 1024 * 1024);
               google::base::SetLogger(FLAGS_minloglevel, async_logger);
               async_logger->Start();
            }

           void StopLogger() {
               if (async_logger != nullptr) {
                   async_logger->Stop();
               }
           }
        }  // namespace

        void OnShutdown(int sig) {
            (void)sig;
            if (GetState() != STATE_SHUTDOWN) {
                SetState(STATE_SHUTTING_DOWN);
            }
        }

        void ExitHandle() { Clear(); }

        bool Init(const char* binary_name) {
            std::lock_guard<std::mutex> lg(g_mutex);
            if (GetState() != STATE_UNINITIALIZED) {
                return false;
            }

            InitLogger(binary_name);
            std::signal(SIGINT, OnShutdown);
            // Register exit handlers
            if (!g_atexit_registered) {
                if (std::atexit(ExitHandle) != 0) {
                AERROR << "Register exit handle failed";
                return false;
                }
                AINFO << "Register exit handle succ.";
                g_atexit_registered = true;
            }
            SetState(STATE_INITIALIZED);
            return true;
        }

        void Clear() {
            std::lock_guard<std::mutex> lg(g_mutex);
            if (GetState() == STATE_SHUTDOWN || GetState() == STATE_UNINITIALIZED) {
                return;
            }
           TaskManager::CleanUp();
           TimerManager::CleanUp();
           scheduler::CleanUp();
           //transport::Transport::CleanUp();
           StopLogger();
           SetState(STATE_SHUTDOWN);
        }

    }  // namespace cyber
}  // namespace apollo
