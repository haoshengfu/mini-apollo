#ifndef CYBER_TASK_TASK_MANAGER_H_
#define CYBER_TASK_TASK_MANAGER_H_

#include <atomic>
#include <future>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "../common/macros.h"
#include "../base/bounded_queue.h"
#include "../scheduler/scheduler_factory.h"

namespace apollo {
namespace cyber {

class TaskManager {
 public:
  virtual ~TaskManager();

  void Shutdown();

  template <typename F, typename... Args>
  auto Enqueue(F&& func, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type> {

    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(func), std::forward<Args>(args)...));
    if (!stop_.load()) {
      task_queue_->Enqueue([task]() { (*task)(); });
      for (auto& task : tasks_) {
        //scheduler::Instance()->NotifyTask(task);
      } 
    }
    std::future<return_type> res(task->get_future());
    return res;
  }

 private:
  uint32_t num_threads_ = 0;
  uint32_t task_queue_size_ = 1000;
  std::atomic<bool> stop_ = {false};
  std::vector<uint64_t> tasks_;
  std::shared_ptr<base::BoundedQueue<std::function<void()>>> task_queue_;
  DECLARE_SINGLETON(TaskManager);
};

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_TASK_TASK_MANAGER_H_
