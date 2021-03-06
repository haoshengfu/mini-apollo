
#ifndef CYBER_CLASS_LOADER_CLASS_LOADER_UTILITY_H_
#define CYBER_CLASS_LOADER_CLASS_LOADER_UTILITY_H_

#include <Poco/SharedLibrary.h>
#include <cassert>
#include <cstdio>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>
#include <iostream>

#include "class_factory.h"
#include "../../common/log.h"

/**
 *  class register implement
 */
namespace apollo {
namespace cyber {
namespace class_loader {

class ClassLoader;

namespace utility {

using PocoLibraryPtr = std::shared_ptr<Poco::SharedLibrary>;
//����ع���
using ClassClassFactoryMap =
    std::map<std::string, utility::AbstractClassFactoryBase*>;
//�������й���
using BaseToClassFactoryMapMap = std::map<std::string, ClassClassFactoryMap>;
using LibpathPocolibVector =
    std::vector<std::pair<std::string, PocoLibraryPtr>>;
using ClassFactoryVector = std::vector<AbstractClassFactoryBase*>;

BaseToClassFactoryMapMap& GetClassFactoryMapMap();
std::recursive_mutex& GetClassFactoryMapMapMutex();
LibpathPocolibVector& GetLibPathPocoShareLibVector();
std::recursive_mutex& GetLibPathPocoShareLibMutex();
ClassClassFactoryMap& GetClassFactoryMapByBaseClass(
    const std::string& typeid_base_class_name);
std::string GetCurLoadingLibraryName();
void SetCurLoadingLibraryName(const std::string& library_name);
ClassLoader* GetCurActiveClassLoader();
void SetCurActiveClassLoader(ClassLoader* loader);
bool IsLibraryLoaded(const std::string& library_path, ClassLoader* loader);
bool IsLibraryLoadedByAnybody(const std::string& library_path);
bool LoadLibrary(const std::string& library_path, ClassLoader* loader);
void UnloadLibrary(const std::string& library_path, ClassLoader* loader);
template <typename Derived, typename Base>
void RegisterClass(const std::string& class_name,
                   const std::string& base_class_name);
template <typename Base>
Base* CreateClassObj(const std::string& class_name, ClassLoader* loader);
template <typename Base>
std::vector<std::string> GetValidClassNames(ClassLoader* loader);

//�������������ʱע��,Ԥ����׶η���������main��
template <typename Derived, typename Base>
void RegisterClass(const std::string& class_name,
                   const std::string& base_class_name) {

  // ���������Ĺ�����
  utility::AbstractClassFactory<Base>* new_class_factrory_obj =
      new utility::ClassFactory<Derived, Base>(class_name, base_class_name);
  // �����������������������ʱ��nullptr���ؼ����ڹ���static��ClassLoader����
  new_class_factrory_obj->AddOwnedClassLoader(GetCurActiveClassLoader());
  // ���ü��ؿ��·������ʱҲ��nullptr...
  new_class_factrory_obj->SetRelativeLibraryPath(GetCurLoadingLibraryName());

  GetClassFactoryMapMapMutex().lock();
  //new���¹����������뵽factory_map��ͳһ����������static�ֵ�BaseToClassFactoryMapMap��value. ���û�ж�Ӧ�Ĺ������򴴽���factoryMapMap[base_class_name] = ClassClassFactoryMap();
  ClassClassFactoryMap& factory_map = GetClassFactoryMapByBaseClass(typeid(Base).name());
  factory_map[class_name] = new_class_factrory_obj;

  GetClassFactoryMapMapMutex().unlock();
}

template <typename Base>
Base* CreateClassObj(const std::string& class_name, ClassLoader* loader) {
  GetClassFactoryMapMapMutex().lock();
  ClassClassFactoryMap& factoryMap =
      GetClassFactoryMapByBaseClass(typeid(Base).name());
  AbstractClassFactory<Base>* factory = nullptr;
  if (factoryMap.find(class_name) != factoryMap.end()) {
    factory = dynamic_cast<utility::AbstractClassFactory<Base>*>(
        factoryMap[class_name]);
  }
  GetClassFactoryMapMapMutex().unlock();

  Base* classobj = nullptr;
  if (factory && factory->IsOwnedBy(loader)) {
    classobj = factory->CreateObj();
  }

  return classobj;
}

template <typename Base>
std::vector<std::string> GetValidClassNames(ClassLoader* loader) {
  std::lock_guard<std::recursive_mutex> lck(GetClassFactoryMapMapMutex());

  ClassClassFactoryMap& factoryMap =
      GetClassFactoryMapByBaseClass(typeid(Base).name());
  std::vector<std::string> classes;
  for (auto& class_factory : factoryMap) {
    AbstractClassFactoryBase* factory = class_factory.second;
    if (factory && factory->IsOwnedBy(loader)) {
      classes.emplace_back(class_factory.first);
    }
  }

  return classes;
}

}  // End namespace utility
}  // End namespace class_loader
}  // namespace cyber
}  // namespace apollo
#endif  // CYBER_CLASS_LOADER_CLASS_LOADER_UTILITY_H_
