#ifndef __ABSTRACTPLUGIN_H__
#define __ABSTRACTPLUGIN_H__

#include <string>

#ifdef  WIN32
#ifdef  ABSTRACT_PLUGIN
#define ABSTRACT_PLUGIN_LIB __declspec(dllexport)
#else
#define ABSTRACT_PLUGIN_LIB __declspec(dllimport)
#endif
#else
#define ABSTRACT_PLUGIN_LIB 
#endif

class ABSTRACT_PLUGIN_LIB AbstractPlugin
{
public:
    AbstractPlugin(){}
    virtual ~AbstractPlugin(){}
    //virtual std::string name() const;
    //virtual bool init();
};
#endif

