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

//��POCO�ṩ�ĺ����������嵥  
//�����չ��ʵ���Ǹ������������ú�����POCO�ڼ���dllʱ�Զ����ã�����嵥�ļ���  
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


