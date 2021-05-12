#ifndef CYBER_CLASS_LOADER_CLASS_LOADER_REGISTER_MACRO_H_
#define CYBER_CLASS_LOADER_CLASS_LOADER_REGISTER_MACRO_H_

#include "utility/class_loader_utility.h"
#include <iostream>


#define CLASS_LOADER_REGISTER_CLASS_INTERNAL(Derived, Base, UniqueID)         \
  namespace {                                                                 \
  struct ProxyType##UniqueID {                                                \
    ProxyType##UniqueID() {                                                   \
      apollo::cyber::class_loader::utility::RegisterClass<Derived, Base>( \
          #Derived, #Base);                                                  \
      AINFO << #Derived << " registed!";                                     \
    }                                                                         \
  };                                                                          \
  static ProxyType##UniqueID g_register_class_##UniqueID;                     \
  }

#define CLASS_LOADER_REGISTER_CLASS_INTERNAL_1(Derived, Base, UniqueID) \
  CLASS_LOADER_REGISTER_CLASS_INTERNAL(Derived, Base, UniqueID)

// register class macro
#define CLASS_LOADER_REGISTER_CLASS(Derived, Base) \
  CLASS_LOADER_REGISTER_CLASS_INTERNAL_1(Derived, Base, __COUNTER__)

#endif  // CYBER_CLASS_LOADER_CLASS_LOADER_REGISTER_MACRO_H_
