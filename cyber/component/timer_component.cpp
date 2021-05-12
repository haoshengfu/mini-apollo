
#include "timer_component.h"
#include "../timer/timer.h"

namespace apollo {
namespace cyber {

TimerComponent::TimerComponent() {}

TimerComponent::~TimerComponent() {}

bool TimerComponent::Process() {
  if (is_shutdown_.load()) {
    return true;
  }
  return Proc();
}

bool TimerComponent::Initialize(const TimerComponentConfig& config) {
  if (!config.has_name() || !config.has_interval()) {
    AERROR << "Missing required field in config file.";
    return false;
  }
  node_.reset(new Node(config.name()));
  LoadConfigFiles(config);
  if (!Init()) {
    return false;
  }

  std::weak_ptr<TimerComponent> self =
      std::dynamic_pointer_cast<TimerComponent>(shared_from_this());
  auto func = [self]() {
    auto ptr = self.lock();
    if (ptr) {
      ptr->Proc();
    }
  };
  timer_.reset(new Timer(config.interval(), func, false));
  timer_->Start();
  return true;
}

}  // namespace cyber
}  // namespace apollo
