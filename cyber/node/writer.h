#ifndef CYBER_NODE_WRITER_H_
#define CYBER_NODE_WRITER_H_

#include <memory>
#include <string>
#include <vector>

#include "../common/log.h"
#include "../node/writer_base.h"
#include "../proto/topology_change.pb.h"
#include "../transport/transport.h"

namespace apollo {
namespace cyber {

template <typename MessageT>
class Writer : public WriterBase {
 public:
  using TransmitterPtr = std::shared_ptr<transport::Transmitter<MessageT>>;

  explicit Writer(const proto::RoleAttributes& role_attr);
  virtual ~Writer();

  bool Init() override;
  void Shutdown() override;

  virtual bool Write(const MessageT& msg);
  virtual bool Write(const std::shared_ptr<MessageT>& msg_ptr);

  bool HasReader() override;
  void GetReaders(std::vector<proto::RoleAttributes>* readers) override;

 private:
  void OnChannelChange(const proto::ChangeMsg& change_msg);

  TransmitterPtr transmitter_;
};

template <typename MessageT>
Writer<MessageT>::Writer(const proto::RoleAttributes& role_attr)
    : WriterBase(role_attr), transmitter_(nullptr) {}

template <typename MessageT>
Writer<MessageT>::~Writer() {
  Shutdown();
}

template <typename MessageT>
bool Writer<MessageT>::Init() {
  {
    std::lock_guard<std::mutex> g(lock_);
    if (init_) { return true; }
    transmitter_ = transport::Transport::Instance()->
      CreateTransmitter<MessageT>(role_attr_);
    if (transmitter_ == nullptr) { return false; }
    init_ = true;
  }
  this->role_attr_.set_id(transmitter_->id().HashValue());
  return true;
}

template <typename MessageT>
void Writer<MessageT>::Shutdown() {
  {
    std::lock_guard<std::mutex> g(lock_);
    if (!init_) { return; }
    init_ = false;
  }
  transmitter_ = nullptr;
}

template <typename MessageT>
bool Writer<MessageT>::Write(const MessageT& msg) {
  RETURN_VAL_IF(!WriterBase::IsInit(), false);
  auto msg_ptr = std::make_shared<MessageT>(msg);
  return Write(msg_ptr);
}

template <typename MessageT>
bool Writer<MessageT>::Write(const std::shared_ptr<MessageT>& msg_ptr) {
  RETURN_VAL_IF(!WriterBase::IsInit(), false);
  return transmitter_->Transmit(msg_ptr);
}

template <typename MessageT>
void Writer<MessageT>::OnChannelChange(const proto::ChangeMsg& change_msg) {
  if (change_msg.role_type() != proto::RoleType::ROLE_READER) {
    return;
  }

  auto& reader_attr = change_msg.role_attr();
  if (reader_attr.channel_name() != this->role_attr_.channel_name()) {
    return;
  }

  auto operate_type = change_msg.operate_type();
  if (operate_type == proto::OperateType::OPT_JOIN) {
    transmitter_->Enable(reader_attr);
  } else {
    transmitter_->Disable(reader_attr);
  }
}

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_NODE_WRITER_H_
