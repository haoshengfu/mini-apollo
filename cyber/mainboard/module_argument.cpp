//
// Created by seaside on 2021/4/20.
//

#include "module_argument.h"

#include <getopt.h>
#include <libgen.h>

using apollo::cyber::common::GlobalData;

namespace apollo {
    namespace cyber {
        namespace mainboard {

            void ModuleArgument::ParseArgument(const int argc, char* const argv[]) {
                binary_name_ = std::string(basename(argv[0]));
                GetOptions(argc, argv);
                if (process_group_.empty()) {
                    process_group_ = DEFAULT_process_group_;
                }
                if (sched_name_.empty()) {
                    sched_name_ = DEFAULT_sched_name_;
                }

                GlobalData::Instance()->SetProcessGroup(process_group_);
                GlobalData::Instance()->SetSchedName(sched_name_);
                 AINFO << "binary_name_ is " << binary_name_ << ", process_group_ is "
                       << process_group_ << ", has " << dag_conf_list_.size() << " dag conf";
                 for (std::string& dag : dag_conf_list_) {
                     AINFO << "dag_conf: " << dag;
                 }
            }

            void ModuleArgument::GetOptions(const int argc, char* const argv[]) {
                opterr = 0;  // extern int opterr
                int long_index = 0;
                const std::string short_opts = "hd:p:s:";
                static const struct option long_opts[] = {
                        {"help", no_argument, nullptr, 'h'},
                        {"dag_conf", required_argument, nullptr, 'd'},
                        {"process_name", required_argument, nullptr, 'p'},
                        {"sched_name", required_argument, nullptr, 's'},
                        {NULL, no_argument, nullptr, 0}};

                // log command for info
                std::string cmd("");
                for (int i = 0; i < argc; ++i) {
                    cmd += argv[i];
                    cmd += " ";
                }
                AINFO << "command: " << cmd;

                do {
                    int opt =
                            getopt_long(argc, argv, short_opts.c_str(), long_opts, &long_index);

                    if (opt == -1) {
                        break;
                    }

                    switch (opt) {

                        case 'd':
                            dag_conf_list_.emplace_back(std::string(optarg));
                            for (int i = optind; i < argc; i++) {
                                if (*argv[i] != '-') {
                                    dag_conf_list_.emplace_back(std::string(argv[i]));
                                } else {
                                    break;
                                }
                            }
                            break;
                        case 'p':
                            process_group_ = std::string(optarg);
                            break;
                        case 's':
                            sched_name_ = std::string(optarg);
                            break;
                        case 'h':
                            exit(0);
                        default:
                            break;
                    }
                } while (true);
            }

        }  // namespace mainboard
    }  // namespace cyber
}  // namespace apollo

