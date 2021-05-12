/*
������������һС�ڵ���⣬Reader ����ֻ������ʵģʽ�±���������Ȼ�Ѿ�˵���˴��������Ǿ�������һ�� Reader �Ĵ������̰�?���� Reader ��Ĺ��캯���У������ Blocker ��ԱҲ���й��죬��ߵĹ��̺ܼ򵥡�����һ���� Reader ���г�ʼ�������� Reader::Init() ����ʱ������͸�����������

�����ص��������� Reader::Enqueue() + ����Ļص�������װ��һ���µĻص����� func
������ĳ�ʼ�����̵���󼸲����ƣ�Reader ��ĳ�ʼ���У�Ҳ�ó��� Scheduler �࣬������ DataVisitor ����ͨ����ƪ�������ϸ��������Э�̹������󣬲������Ǵ�����һ������Ŀ�ľ��ǽ���һ���з�װ�õĻص����� func ��װΪЭ�̣������ݵ����㷨�İ��ţ��ں��ʵ�ʱ����á����⻹�Ѷ�Ӧ��Э�����ּ�¼�� croutine_name_ ��
���� Reader ��������ԣ��ӽ����������� ReceiverManager<M> ��ȡ�����Ӧ�� Receiver ���󣬸ö������ڽ�����Ϣ
�� service_discovery::TopologyManager �����õ��ŵ������� channel_manager ��������� Reader ������뵽���˽ṹ��
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
