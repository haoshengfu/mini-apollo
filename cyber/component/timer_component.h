#ifndef CYBER_COMPONENT_TIMER_COMPONENT_H_
#define CYBER_COMPONENT_TIMER_COMPONENT_H_

#include <memory>

#include "component_base.h"

namespace apollo {
namespace cyber {

class Timer;

class TimerComponent : public ComponentBase {
 public:
  TimerComponent();
  ~TimerComponent() override;

  bool Initialize(const TimerComponentConfig& config) override;
  bool Process();

 private:
  virtual bool Proc() = 0;
  virtual bool Init() = 0;

  uint64_t interval_ = 0;
  std::unique_ptr<Timer> timer_;
};

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_COMPONENT_TIMER_COMPONENT_H_
