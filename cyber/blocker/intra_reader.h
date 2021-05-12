/******************************************************************************
先再次介绍一下 IntraReader 类与 IntraWriter 类。它们俩都是在模拟模式下才会出现的对象。代码文件位于 cyber/blocker/intra_reader.h 和 cyber/bolcker/intra_writer.h，它们的实现与 BlockerManager 类关系密切，这也是为何它们的代码文件会位于 cyber/blocker 而不是和 Reader 一样位于 cyber/node 的重要原因（也是为什么我选择在这里讲它们俩的原因?）。它们分别继承自 Reader 和 Writer 。因为这是模拟模式下创建的对象，所以它们不会去创建协程，去获取传感器的数据。

再来看看为什么称 BlockerManager 为模拟模式的助手。因为 IntraReader 和 IntraWriter 都或多或少与它有点关系。
看看它们的初始化，IntraReader 类重载了 Reader::Init() 函数。在初始化中，它不会创建协程，而是直接调用 BlockerManager::Subscribe()，把 IntraReader::OnMessage() 函数（记录时间并调用 IntraReader 创建时传入的回调函数）注册为该信道对应 Blocker 的回调函数。
*****************************************************************************/

#ifndef CYBER_BLOCKER_INTRA_READER_H_
#define CYBER_BLOCKER_INTRA_READER_H_

#include <functional>
#include <list>
#include <memory>

#include "blocker_manager.h"
#include "../common/log.h"
#include "../node/reader.h"
#include "../time/time.h"

namespace apollo {
namespace cyber {
namespace blocker {

template <typename MessageT>
class IntraReader : public apollo::cyber::Reader<MessageT> {
 public:

  using MessagePtr = std::shared_ptr<MessageT>;
  using Callback = std::function<void(const std::shared_ptr<MessageT>&)>;
  using Iterator =
      typename std::list<std::shared_ptr<MessageT>>::const_iterator;

  IntraReader(const proto::RoleAttributes& attr, const Callback& callback);
  virtual ~IntraReader();

  bool Init() override;
  void Shutdown() override;

  void ClearData() override;
  void Observe() override;
  bool Empty() const override;
  bool HasReceived() const override;

  void Enqueue(const std::shared_ptr<MessageT>& msg) override;
  void SetHistoryDepth(const uint32_t& depth) override;
  uint32_t GetHistoryDepth() const override;
  std::shared_ptr<MessageT> GetLatestObserved() const override;
  std::shared_ptr<MessageT> GetOldestObserved() const override;
  Iterator Begin() const; //override;
  Iterator End() const; //override;

 private:
  void OnMessage(const MessagePtr& msg_ptr);

  Callback msg_callback_;
};

template <typename MessageT>
IntraReader<MessageT>::IntraReader(const proto::RoleAttributes& attr,
                                   const Callback& callback)
    : Reader<MessageT>(attr), msg_callback_(callback) {}

template <typename MessageT>
IntraReader<MessageT>::~IntraReader() {
  Shutdown();
}

template <typename MessageT>
bool IntraReader<MessageT>::Init() {
  if (this->init_.exchange(true)) {
    return true;
  }
  return BlockerManager::Instance()->Subscribe<MessageT>(
      this->role_attr_.channel_name(), this->role_attr_.qos_profile().depth(),
      this->role_attr_.node_name(), std::bind(&IntraReader<MessageT>::OnMessage,
                                              this, std::placeholders::_1));
}

template <typename MessageT>
void IntraReader<MessageT>::Shutdown() {
  if (!this->init_.exchange(false)) {
    return;
  }
  BlockerManager::Instance()->Unsubscribe<MessageT>(
      this->role_attr_.channel_name(), this->role_attr_.node_name());
}

template <typename MessageT>
void IntraReader<MessageT>::ClearData() {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  if (blocker != nullptr) {
    blocker->ClearObserved();
    blocker->ClearPublished();
  }
}

template <typename MessageT>
void IntraReader<MessageT>::Observe() {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  if (blocker != nullptr) {
    blocker->Observe();
  }
}

template <typename MessageT>
bool IntraReader<MessageT>::Empty() const {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  if (blocker != nullptr) {
    return blocker->IsObservedEmpty();
  }
  return true;
}

template <typename MessageT>
bool IntraReader<MessageT>::HasReceived() const {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  if (blocker != nullptr) {
    return !blocker->IsPublishedEmpty();
  }
  return false;
}

template <typename MessageT>
void IntraReader<MessageT>::Enqueue(const std::shared_ptr<MessageT>& msg) {
  BlockerManager::Instance()->Publish<MessageT>(this->role_attr_.channel_name(),
                                                msg);
}

template <typename MessageT>
void IntraReader<MessageT>::SetHistoryDepth(const uint32_t& depth) {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  if (blocker != nullptr) {
    blocker->set_capacity(depth);
  }
}

template <typename MessageT>
uint32_t IntraReader<MessageT>::GetHistoryDepth() const {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  if (blocker != nullptr) {
    return static_cast<uint32_t>(blocker->capacity());
  }
  return 0;
}

template <typename MessageT>
std::shared_ptr<MessageT> IntraReader<MessageT>::GetLatestObserved() const {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  if (blocker != nullptr) {
    return blocker->GetLatestObservedPtr();
  }
  return nullptr;
}

template <typename MessageT>
std::shared_ptr<MessageT> IntraReader<MessageT>::GetOldestObserved() const {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  if (blocker != nullptr) {
    return blocker->GetOldestObservedPtr();
  }
  return nullptr;
}

template <typename MessageT>
auto IntraReader<MessageT>::Begin() const -> Iterator {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  ACHECK(blocker != nullptr);
  return blocker->ObservedBegin();
}

template <typename MessageT>
auto IntraReader<MessageT>::End() const -> Iterator {
  auto blocker = BlockerManager::Instance()->GetBlocker<MessageT>(
      this->role_attr_.channel_name());
  ACHECK(blocker != nullptr);
  return blocker->ObservedBegin();
}

template <typename MessageT>
void IntraReader<MessageT>::OnMessage(const MessagePtr& msg_ptr) {
  this->second_to_lastest_recv_time_sec_ = this->latest_recv_time_sec_;
  this->latest_recv_time_sec_ = apollo::cyber::Time::Now().ToSecond();
  if (msg_callback_ != nullptr) {
    msg_callback_(msg_ptr);
  }
}

}  // namespace blocker
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_BLOCKER_INTRA_READER_H_
