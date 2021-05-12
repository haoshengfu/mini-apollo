#include "test.h"  

bool CommonComponentSample::Init() {
  AINFO << "Commontest component init";
  return true;
}

bool CommonComponentSample::Proc(const std::shared_ptr<Driver>& msg0) {
  AINFO << "Start common component Proc [" << msg0->msg_id() << "]";
  return true;
}


CommonComponentSample::CommonComponentSample()
{
    std::cout << "CommonComponentSample!!!" << std::endl;
}
