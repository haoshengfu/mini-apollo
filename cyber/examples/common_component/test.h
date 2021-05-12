#ifndef __PLUGIN_A_H__
#define __PLUGIN_A_H__  

#include <string>
#include <iostream>  

#include "../../class_loader/class_loader.h"
#include "../../component/component.h"
#include "../proto/examples.pb.h"

using apollo::cyber::examples::proto::Driver;
using apollo::cyber::Component;
using apollo::cyber::ComponentBase;


class CommonComponentSample : public Component<Driver>{
 public:
  CommonComponentSample();
  bool Init();
  bool Proc(const std::shared_ptr<Driver>& msg0) override;
};

CYBER_REGISTER_COMPONENT(CommonComponentSample)


#endif

