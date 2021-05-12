//
// Created by seaside on 2021/4/20.
//

#ifndef MINIAPOLLO_GLOBAL_DATA_H
#define MINIAPOLLO_GLOBAL_DATA_H



#include <string>
#include <unordered_map>

#include "../base/atomic_hash_map.h"
#include "../base/atomic_rw_lock.h"
#include "log.h"
#include "macros.h"
#include "util.h"
#include "../proto/cyber_conf.pb.h"
namespace apollo {
    namespace cyber {
        namespace common {

            using ::apollo::cyber::base::AtomicHashMap;
            using ::apollo::cyber::proto::CyberConfig;

            class GlobalData {
            public:
                ~GlobalData();

                int ProcessId() const;

                void SetProcessGroup(const std::string& process_group);
                const std::string& ProcessGroup() const;

                void SetSchedName(const std::string& sched_name);
                const std::string& SchedName() const;

                const std::string& HostIp() const;

                const std::string& HostName() const;

                const CyberConfig& Config() const;

                void EnableSimulationMode();
                void DisableSimulationMode();

                bool IsRealityMode() const;

                static uint64_t GenerateHashId(const std::string& name) {
                    return common::Hash(name);
                }

                static uint64_t RegisterNode(const std::string& node_name);
                static std::string GetNodeById(uint64_t id);

                static uint64_t RegisterChannel(const std::string& channel);
                static std::string GetChannelById(uint64_t id);

                static uint64_t RegisterService(const std::string& service);
                static std::string GetServiceById(uint64_t id);

                static uint64_t RegisterTaskName(const std::string& task_name);
                static std::string GetTaskNameById(uint64_t id);

            private:
                void InitHostInfo();
                bool InitConfig();

                 // global config
                 CyberConfig config_;

                // host info
                std::string host_ip_;
                std::string host_name_;

                // process info
                int process_id_;
                std::string process_group_;

                // sched policy info
                std::string sched_name_ = "CYBER_DEFAULT";

                // run mode
                bool is_reality_mode_;

                 static AtomicHashMap<uint64_t, std::string, 512> node_id_map_;
                 static AtomicHashMap<uint64_t, std::string, 256> channel_id_map_;
                 static AtomicHashMap<uint64_t, std::string, 256> service_id_map_;
                 static AtomicHashMap<uint64_t, std::string, 256> task_id_map_;

                DECLARE_SINGLETON(GlobalData)
            };

        }  // namespace common
    }  // namespace cyber
}  // namespace apollo


#endif //MINIAPOLLO_GLOBAL_DATA_H
