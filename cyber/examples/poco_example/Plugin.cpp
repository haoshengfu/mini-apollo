#include "Plugin.h"  
#include "Poco/ClassLibrary.h"  


PluginA::PluginA()
{
    std::cout << "Plugin!!!" << std::endl;
}

PluginA::~PluginA()
{

}
bool PluginA::init(){return true;}

// std::string PluginA::name() const
// {
//     return "This is PluginA";
// }

//用POCO提供的宏来生成类清单  
//这个宏展开实际是个函数声明，该函数由POCO在加载dll时自动调用，完成清单的加载  
// POCO_BEGIN_MANIFEST(AbstractPlugin)
// POCO_EXPORT_CLASS(PluginA)
// POCO_END_MANIFEST

// // optional set up and clean up functions  
// void pocoInitializeLibrary()
// {
//     std::cout << "PluginLibrary initializing" << std::endl;
// }

// void pocoUninitializeLibrary()
// {
//     std::cout << "PluginLibrary uninitializing" << std::endl;
// }


