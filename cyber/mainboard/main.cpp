#include "../init.h"
#include "../common/global_data.h"
#include "../common/log.h"
#include "module_argument.h"
#include "module_controller.h"
#include "../state.h"
#include "gflags/gflags.h"

using apollo::cyber::mainboard::ModuleArgument;
using apollo::cyber::mainboard::ModuleController;


int main(int argc, char** argv)
{
   google::SetUsageMessage("we use this program to load dag and run user apps.");
   // parse the argument
   ModuleArgument module_args;
   module_args.ParseArgument(argc, argv);

    //initialize cyber
    apollo::cyber::Init(argv[0]);


    // start module
    ModuleController controller(module_args);
    if (!controller.Init()) {
        controller.Clear();
        AERROR << "module start error.";
        return -1;
    }
    return 0;
}
