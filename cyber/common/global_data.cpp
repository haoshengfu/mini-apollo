//
// Created by seaside on 2021/4/20.
//

#include "global_data.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <limits.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <functional>

#include "environment.h"
#include "file.h"

namespace apollo {
    namespace cyber {
        namespace common {

            // AtomicHashMap<uint64_t, std::string, 512> GlobalData::node_id_map_;
            // AtomicHashMap<uint64_t, std::string, 256> GlobalData::channel_id_map_;
            // AtomicHashMap<uint64_t, std::string, 256> GlobalData::service_id_map_;
            // AtomicHashMap<uint64_t, std::string, 256> GlobalData::task_id_map_;

            namespace {
                const char* empty_str_ = "";
                char* program_path() {
                    char* path = reinterpret_cast<char*>(malloc(PATH_MAX));
                    if (path != nullptr) {
                        auto len = readlink("/proc/self/exe", path, PATH_MAX);
                        if (len == -1) {
                            free(path);
                            return nullptr;
                        }
                        path[len] = '\0';
                    }
                    return path;
                }
            }  // namespace

            GlobalData::GlobalData() {
                InitHostInfo();
                //CHECK(InitConfig());
                process_id_ = getpid();
                char* prog_path = program_path();
                if (prog_path) {
                    process_group_ = GetFileName(prog_path) + "_" + std::to_string(process_id_);
                    free(prog_path);
                } else {
                    process_group_ = "cyber_default_" + std::to_string(process_id_);
                }
                // is_reality_mode_ = (config_.has_run_mode_conf() &&
                //                     config_.run_mode_conf().run_mode() ==
                //                     apollo::cyber::proto::RunMode::MODE_SIMULATION)
                //                    ? false
                //                    : true;
                is_reality_mode_ = false;
                const char* run_mode_val = ::getenv("CYBER_RUN_MODE");
                if (run_mode_val != nullptr) {
                    std::string run_mode_str(run_mode_val);
                    if (run_mode_str == "simulation") {
                        is_reality_mode_ = false;
                    }
                }
            }

            GlobalData::~GlobalData() {}

            int GlobalData::ProcessId() const { return process_id_; }

            void GlobalData::SetProcessGroup(const std::string& process_group) {
                process_group_ = process_group;
            }
            const std::string& GlobalData::ProcessGroup() const { return process_group_; }

            void GlobalData::SetSchedName(const std::string& sched_name) {
                sched_name_ = sched_name;
            }
            const std::string& GlobalData::SchedName() const { return sched_name_; }

            const std::string& GlobalData::HostIp() const { return host_ip_; }

            const std::string& GlobalData::HostName() const { return host_name_; }

            void GlobalData::EnableSimulationMode() { is_reality_mode_ = false; }

            void GlobalData::DisableSimulationMode() { is_reality_mode_ = true; }

            bool GlobalData::IsRealityMode() const { return is_reality_mode_; }

            void GlobalData::InitHostInfo() {
                char host_name[1024];
                gethostname(host_name, 1024);
                host_name_ = host_name;

                host_ip_ = "127.0.0.1";

                // if we have exported a non-loopback CYBER_IP, we will use it firstly,
                // otherwise, we try to find first non-loopback ipv4 addr.
                const char* ip_env = getenv("CYBER_IP");
                if (ip_env != nullptr) {
                    // maybe we need to verify ip_env
                    std::string ip_env_str(ip_env);
                    std::string starts = ip_env_str.substr(0, 3);
                    if (starts != "127") {
                        host_ip_ = ip_env_str;
                        AINFO << "host ip: " << host_ip_;
                        return;
                    }
                }

                ifaddrs* ifaddr = nullptr;
                if (getifaddrs(&ifaddr) != 0) {
                    AERROR << "getifaddrs failed, we will use 127.0.0.1 as host ip.";
                    return;
                }
                for (ifaddrs* ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
                    if (ifa->ifa_addr == nullptr) {
                        continue;
                    }
                    int family = ifa->ifa_addr->sa_family;
                    if (family != AF_INET) {
                        continue;
                    }
                    char addr[NI_MAXHOST] = {0};
                    if (getnameinfo(ifa->ifa_addr, sizeof(sockaddr_in), addr, NI_MAXHOST, NULL,
                                    0, NI_NUMERICHOST) != 0) {
                        continue;
                    }
                    std::string tmp_ip(addr);
                    std::string starts = tmp_ip.substr(0, 3);
                    if (starts != "127") {
                        host_ip_ = tmp_ip;
                        break;
                    }
                }
                freeifaddrs(ifaddr);
                AINFO << "host ip: " << host_ip_;
            }

            bool GlobalData::InitConfig() {
                auto config_path = GetAbsolutePath(WorkRoot(), "conf/cyber.pb.conf");
                if (!GetProtoFromFile(config_path, &config_)) {
                    AERROR << "read cyber default conf failed!";
                    return false;
                }

                return true;
            }

            const CyberConfig& GlobalData::Config() const { return config_; }

            uint64_t GlobalData::RegisterNode(const std::string& node_name) {
                auto id = Hash(node_name);
//                if (node_id_map_.Has(id)) {
//                    std::string* name = nullptr;
//                    node_id_map_.Get(id, &name);
//                    CHECK(node_name == *name) << " Node name hash collision: " << node_name
//                                              << " <=> " << *name;
//                }
//                node_id_map_.Set(id, node_name);
                return id;
            }

            std::string GlobalData::GetNodeById(uint64_t id) {
//                std::string* node_name = nullptr;
//                if (node_id_map_.Get(id, &node_name)) {
//                    return *node_name;
//                }
                return empty_str_;
            }

            uint64_t GlobalData::RegisterChannel(const std::string& channel) {
                auto id = Hash(channel);
//                if (channel_id_map_.Has(id)) {
//                    std::string* name = nullptr;
//                    channel_id_map_.Get(id, &name);
//                    CHECK(channel == *name) << "Channel name hash collision: " << channel
//                                            << " <=> " << *name;
//                }
//                channel_id_map_.Set(id, channel);
                return id;
            }

            std::string GlobalData::GetChannelById(uint64_t id) {
//                std::string* channel = nullptr;
//                if (channel_id_map_.Get(id, &channel)) {
//                    return *channel;
//                }
                return empty_str_;
            }

            uint64_t GlobalData::RegisterService(const std::string& service) {
                auto id = Hash(service);
//                if (service_id_map_.Has(id)) {
//                    std::string* name = nullptr;
//                    service_id_map_.Get(id, &name);
//                    CHECK(service == *name) << "Service name hash collision: " << service
//                                            << " <=> " << *name;
//                }
//                service_id_map_.Set(id, service);
                return id;
            }

            std::string GlobalData::GetServiceById(uint64_t id) {
//                std::string* service = nullptr;
//                if (service_id_map_.Get(id, &service)) {
//                    return *service;
//                }
                return empty_str_;
            }

            uint64_t GlobalData::RegisterTaskName(const std::string& task_name) {
                auto id = Hash(task_name);
//                if (task_id_map_.Has(id)) {
//                    std::string* name = nullptr;
//                    task_id_map_.Get(id, &name);
//                    CHECK(task_name == *name) << "Task name hash collision: " << task_name
//                                              << " <=> " << *name;
//                }
//                task_id_map_.Set(id, task_name);
                return id;
            }

            std::string GlobalData::GetTaskNameById(uint64_t id) {
//                std::string* task_name = nullptr;
//                if (task_id_map_.Get(id, &task_name)) {
//                    return *task_name;
//                }
                return empty_str_;
            }

        }  // namespace common
    }  // namespace cyber
}  // namespace apollo
