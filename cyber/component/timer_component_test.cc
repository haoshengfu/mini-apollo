
#include "timer_component.h"

#include <gtest/gtest.h>
#include <memory>

#include "../init.h"

namespace apollo {
namespace cyber {

static bool ret_proc = true;
static bool ret_init = true;

class Component_Timer : public TimerComponent {
 public:
  Component_Timer() {}
  bool Init() { return ret_init; }
  bool Proc() { return ret_proc; }
};

// TEST(TimerComponent, timertest) {
//   ret_proc = true;
//   ret_init = true;
//   cyber::Init("timer component test");
//   apollo::cyber::proto::TimerComponentConfig compcfg;
//   compcfg.set_name("driver");
//   compcfg.set_interval(100);

//   std::shared_ptr<Component_Timer> com = std::make_shared<Component_Timer>();
//   EXPECT_EQ(true, com->Initialize(compcfg));
//   EXPECT_EQ(true, com->Process());
// }

// TEST(TimerComponentFalse, timerfail) {
//   ret_proc = false;
//   ret_init = false;
//   cyber::Init("timer component test");
//   apollo::cyber::proto::TimerComponentConfig compcfg;
//   compcfg.set_name("driver1");
//   compcfg.set_interval(100);

//   std::shared_ptr<Component_Timer> com = std::make_shared<Component_Timer>();
//   EXPECT_EQ(false, com->Initialize(compcfg));
//   EXPECT_EQ(false, com->Process());
// }
}  // namespace cyber
}  // namespace apollo
