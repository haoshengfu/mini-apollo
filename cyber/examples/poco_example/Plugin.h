#ifndef __PLUGIN_A_H__
#define __PLUGIN_A_H__  

#include "PluginBase.h"
#include <string>
#include <iostream> 

#include "../../class_loader/class_loader.h"
#include "../../component/component.h"
class PluginA : public AbstractPlugin
{
public:
    PluginA();
    ~PluginA();
    bool init();
};

#define CLASS_LOADER_REGISTER_CLASS_INTERNAL(Derived, Base, UniqueID)         \
  namespace {                                                                 \
  struct ProxyType##UniqueID {                                                \
    ProxyType##UniqueID() {                                                   \
      apollo::cyber::class_loader::utility::RegisterClass<Derived, Base>( \
      #Derived, #Base);                                                  \
      std::cout << "Plugin registed! " << UniqueID << std::endl;                                                  \
    }                                                                         \
  };                                                                          \
  static ProxyType##UniqueID g_register_class_##UniqueID;                     \
  }

CLASS_LOADER_REGISTER_CLASS_INTERNAL(PluginA, AbstractPlugin, 111)

static PluginA a;
#endif

