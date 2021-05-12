/*
根据我们在上一小节的理解，Reader 对象只会在真实模式下被创建。既然已经说到了创建，我们就先来看一下 Reader 的创建过程吧?。在 Reader 类的构造函数中，其类的 Blocker 成员也进行构造，这边的过程很简单。而进一步对 Reader 进行初始化，调用 Reader::Init() 函数时，情况就复杂了起来：

创建回调函数。用 Reader::Enqueue() + 传入的回调函数封装出一个新的回调函数 func
与组件的初始化过程的最后几步相似，Reader 类的初始化中，也拿出了 Scheduler 类，创建了 DataVisitor 对象（通信下篇会对其详细阐述）和协程工厂对象，并用它们创建了一个任务，目的就是将第一步中封装好的回调函数 func 包装为协程，最后根据调度算法的安排，在合适的时候调用。此外还把对应的协程名字记录到 croutine_name_ 中
根据 Reader 对象的属性，从接收器管理类 ReceiverManager<M> 中取出相对应的 Receiver 对象，该对象用于接受消息
从 service_discovery::TopologyManager 那里拿到信道管理器 channel_manager ，最后把这个 Reader 对象加入到拓扑结构中
*/

#ifndef CYBER_NODE_READER_H_
#define CYBER_NODE_READER_H_

#include <algorithm>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../blocker/blocker.h"
#include "../common/global_data.h"
#include "../croutine/routine_factory.h"
#include "../data/data_visitor.h"
#include "../node/reader_base.h"
#include "../proto/topology_change.pb.h"
#include "../scheduler/scheduler_factory.h"
#include "../time/time.h"
#include "../transport/transport.h"

namespace apollo {
namespace cyber {

template <typename M0>
using CallbackFunc = std::function<void(const std::shared_ptr<M0>&)>;

using proto::RoleType;

const uint32_t DEFAULT_PENDING_QUEUE_SIZE = 1;

template <typename MessageT>
class Reader : public ReaderBase {
 public:
  using BlockerPtr = std::unique_ptr<blocker::Blocker<MessageT>>;
  using ReceiverPtr = std::shared_ptr<transport::Receiver<MessageT>>;
  using Iterator =
      typename std::list<std::shared_ptr<MessageT>>::const_iterator;

  explicit Reader(const proto::RoleAttributes& role_attr,
         const CallbackFunc<MessageT>& reader_func = nullptr,
         uint32_t pending_queue_size = DEFAULT_PENDING_QUEUE_SIZE);
  virtual ~Reader();

  bool Init() override;
  void Shutdown() override;
  void Observe() override;
  void ClearData() override;
  bool HasReceived() const override;
  bool Empty() const override;
  double GetDelaySec() const override;
  uint32_t PendingQueueSize() const override;

  virtual void Enqueue(const std::shared_ptr<MessageT>& msg);
  virtual void SetHistoryDepth(const uint32_t& depth);
  virtual uint32_t GetHistoryDepth() const;
  virtual std::shared_ptr<MessageT> GetLatestObserved() const;
  virtual std::shared_ptr<MessageT> GetOldestObserved() const;

  bool HasWriter() override;
  void GetWriters(std::vector<proto::RoleAttributes>* writers) override;

 protected:
  double latest_recv_time_sec_ = -1.0;
  double second_to_lastest_recv_time_sec_ = -1.0;
  uint32_t pending_queue_size_;

 private:
  void JoinTheTopology();
  void LeaveTheTopology();
  void OnChannelChange(const proto::ChangeMsg& change_msg);

  CallbackFunc<MessageT> reader_func_;
  ReceiverPtr receiver_ = nullptr;
  std::string croutine_name_;
  BlockerPtr blocker_ = nullptr;
};

template <typename MessageT>
Reader<MessageT>::Reader(const proto::RoleAttributes& role_attr,
                         const CallbackFunc<MessageT>& reader_func,
                         uint32_t pending_queue_size)
    : ReaderBase(role_attr), pending_queue_size_(pending_queue_size),
      reader_func_(reader_func) {
  blocker_.reset(new blocker::Blocker<MessageT>(blocker::BlockerAttr(
      role_attr.qos_profile().depth(), role_attr.channel_name())));
}

template <typename MessageT>
Reader<MessageT>::~Reader() {
  Shutdown();
}

template <typename MessageT>
void Reader<MessageT>::Enqueue(const std::shared_ptr<MessageT>& msg) {
  second_to_lastest_recv_time_sec_ = latest_recv_time_sec_;
  latest_recv_time_sec_ = Time::Now().ToSecond();
  blocker_->Publish(msg);
}

template <typename MessageT>
void Reader<MessageT>::Observe() {
  blocker_->Observe();
}

template <typename MessageT>
bool Reader<MessageT>::Init() {
  if (init_.exchange(true)) {
    return true;
  }
  std::function<void(const std::shared_ptr<MessageT>&)> func;
  if (reader_func_ != nullptr) {
    func = [this](const std::shared_ptr<MessageT>& msg) {
      this->Enqueue(msg);
      this->reader_func_(msg);
    };
  } else {
    func = [this](const std::shared_ptr<MessageT>& msg) { this->Enqueue(msg); };
  }
  auto sched = scheduler::Instance();
  croutine_name_ = role_attr_.node_name() + "_" + role_attr_.channel_name();
  auto dv = std::make_shared<data::DataVisitor<MessageT>>(
      role_attr_.channel_id(), pending_queue_size_);
  // Using factory to wrap templates.
  croutine::RoutineFactory factory =
      croutine::CreateRoutineFactory<MessageT>(std::move(func), dv);
  if (!sched->CreateTask(factory, croutine_name_)) {
    AERROR << "Create Task Failed!";
    init_.exchange(false);
    return false;
  }

  receiver_ = ReceiverManager<MessageT>::Instance()->GetReceiver(role_attr_);
  this->role_attr_.set_id(receiver_->id().HashValue());

  return true;
}

template <typename MessageT>
void Reader<MessageT>::Shutdown() {
  if (!init_.exchange(false)) {
    return;
  }
  //LeaveTheTopology();
  receiver_ = nullptr;

  if (!croutine_name_.empty()) {
    scheduler::Instance()->RemoveTask(croutine_name_);
  }
}


template <typename MessageT>
void Reader<MessageT>::OnChannelChange(const proto::ChangeMsg& change_msg) {
  if (change_msg.role_type() != proto::RoleType::ROLE_WRITER) {
    return;
  }

  auto& writer_attr = change_msg.role_attr();
  if (writer_attr.channel_name() != this->role_attr_.channel_name()) {
    return;
  }

  auto operate_type = change_msg.operate_type();
  if (operate_type == proto::OperateType::OPT_JOIN) {
    receiver_->Enable(writer_attr);
  } else {
    receiver_->Disable(writer_attr);
  }
}

template <typename MessageT>
bool Reader<MessageT>::HasReceived() const {
  return !blocker_->IsPublishedEmpty();
}

template <typename MessageT>
bool Reader<MessageT>::Empty() const {
  return blocker_->IsObservedEmpty();
}

template <typename MessageT>
double Reader<MessageT>::GetDelaySec() const {
  if (latest_recv_time_sec_ < 0) {
    return -1.0;
  }
  if (second_to_lastest_recv_time_sec_ < 0) {
    return Time::Now().ToSecond() - latest_recv_time_sec_;
  }
  return std::max((Time::Now().ToSecond() - latest_recv_time_sec_),
                  (latest_recv_time_sec_ - second_to_lastest_recv_time_sec_));
}

template <typename MessageT>
uint32_t Reader<MessageT>::PendingQueueSize() const {
  return pending_queue_size_;
}

template <typename MessageT>
std::shared_ptr<MessageT> Reader<MessageT>::GetLatestObserved() const {
  return blocker_->GetLatestObservedPtr();
}

template <typename MessageT>
std::shared_ptr<MessageT> Reader<MessageT>::GetOldestObserved() const {
  return blocker_->GetOldestObservedPtr();
}

template <typename MessageT>
void Reader<MessageT>::ClearData() {
  blocker_->ClearPublished();
  blocker_->ClearObserved();
}

template <typename MessageT>
void Reader<MessageT>::SetHistoryDepth(const uint32_t& depth) {
  blocker_->set_capacity(depth);
}

template <typename MessageT>
uint32_t Reader<MessageT>::GetHistoryDepth() const {
  return static_cast<uint32_t>(blocker_->capacity());
}


template <typename MessageT>
bool Reader<MessageT>::HasWriter() {
  if (!init_.load()) {
    return false;
  }
  return true;
  //return channel_manager_->HasWriter(role_attr_.channel_name());
}

template <typename MessageT>
void Reader<MessageT>::GetWriters(std::vector<proto::RoleAttributes>* writers) {
  if (writers == nullptr) {
    return;
  }

  if (!init_.load()) {
    return;
  }

  //channel_manager_->GetWritersOfChannel(role_attr_.channel_name(), writers);
}

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_NODE_READER_H_
