#ifndef CYBER_NODE_NODE_CHANNEL_IMPL_H_
#define CYBER_NODE_NODE_CHANNEL_IMPL_H_

#include <memory>
#include <string>
#include "../blocker/intra_reader.h"
#include "../blocker/intra_writer.h"
#include "../common/global_data.h"
#include "../message/message_traits.h"
#include "reader.h"
#include "writer.h"
#include "../proto/run_mode_conf.pb.h"

namespace apollo {
namespace cyber {

class Node;

struct ReaderConfig {
 ReaderConfig() {
   qos_profile.set_history(proto::QosHistoryPolicy::HISTORY_KEEP_LAST);
   qos_profile.set_depth(1);
   qos_profile.set_mps(0);
   qos_profile.set_reliability(
       proto::QosReliabilityPolicy::RELIABILITY_RELIABLE);
   qos_profile.set_durability(proto::QosDurabilityPolicy::DURABILITY_VOLATILE);

   pending_queue_size = DEFAULT_PENDING_QUEUE_SIZE;
 }
 ReaderConfig(const ReaderConfig& other)
     : channel_name(other.channel_name),
       qos_profile(other.qos_profile),
       pending_queue_size(other.pending_queue_size) {}

 std::string channel_name;
 proto::QosProfile qos_profile;
 uint32_t pending_queue_size;
};

class NodeChannelImpl {
 friend class Node;

public:
 explicit NodeChannelImpl(const std::string& node_name)
     : is_reality_mode_(true), node_name_(node_name) {
   node_attr_.set_host_name(common::GlobalData::Instance()->HostName());
   node_attr_.set_host_ip(common::GlobalData::Instance()->HostIp());
   node_attr_.set_process_id(common::GlobalData::Instance()->ProcessId());
   node_attr_.set_node_name(node_name);
   uint64_t node_id = common::GlobalData::RegisterNode(node_name);
   node_attr_.set_node_id(node_id);

   is_reality_mode_ = common::GlobalData::Instance()->IsRealityMode();

 }
 virtual ~NodeChannelImpl() {
 }

 const std::string& NodeName() const { return node_name_; }

public:
 template <typename MessageT>
 auto CreateWriter(const proto::RoleAttributes& role_attr)
     -> std::shared_ptr<Writer<MessageT>>;

 template <typename MessageT>
 auto CreateWriter(const std::string& channel_name)
     -> std::shared_ptr<Writer<MessageT>>;

 template <typename MessageT>
 auto CreateReader(const std::string& channel_name,
                   const CallbackFunc<MessageT>& reader_func)
     -> std::shared_ptr<Reader<MessageT>>;

 template <typename MessageT>
 auto CreateReader(const ReaderConfig& config,
                   const CallbackFunc<MessageT>& reader_func)
     -> std::shared_ptr<Reader<MessageT>>;

 template <typename MessageT>
 auto CreateReader(const proto::RoleAttributes& role_attr,
                   const CallbackFunc<MessageT>& reader_func,
                   uint32_t pending_queue_size = DEFAULT_PENDING_QUEUE_SIZE)
     -> std::shared_ptr<Reader<MessageT>>;

 template <typename MessageT>
 auto CreateReader(const proto::RoleAttributes& role_attr)
     -> std::shared_ptr<Reader<MessageT>>;

 template <typename MessageT>
 void FillInAttr(proto::RoleAttributes* attr);

 bool is_reality_mode_;
 std::string node_name_;
 proto::RoleAttributes node_attr_;
};

template <typename MessageT>
auto NodeChannelImpl::CreateWriter(const proto::RoleAttributes& role_attr)
   -> std::shared_ptr<Writer<MessageT>> {
 if (!role_attr.has_channel_name() || role_attr.channel_name().empty()) {
   AERROR << "Can't create a writer with empty channel name!";
   return nullptr;
 }
 proto::RoleAttributes new_attr(role_attr);
 FillInAttr<MessageT>(&new_attr);

 std::shared_ptr<Writer<MessageT>> writer_ptr = nullptr;
 if (!is_reality_mode_) {
   //writer_ptr = std::make_shared<blocker::IntraWriter<MessageT>>(new_attr);
 } else {
   writer_ptr = std::make_shared<Writer<MessageT>>(new_attr);
 }

 RETURN_VAL_IF_NULL(writer_ptr, nullptr);
 RETURN_VAL_IF(!writer_ptr->Init(), nullptr);
 return writer_ptr;
}

template <typename MessageT>
auto NodeChannelImpl::CreateWriter(const std::string& channel_name)
   -> std::shared_ptr<Writer<MessageT>> {
 proto::RoleAttributes role_attr;
 role_attr.set_channel_name(channel_name);
 return this->CreateWriter<MessageT>(role_attr);
}

template <typename MessageT>
auto NodeChannelImpl::CreateReader(const std::string& channel_name,
                                  const CallbackFunc<MessageT>& reader_func)
   -> std::shared_ptr<Reader<MessageT>> {
 proto::RoleAttributes role_attr;
 role_attr.set_channel_name(channel_name);
 return this->template CreateReader<MessageT>(role_attr, reader_func);
}

template <typename MessageT>
auto NodeChannelImpl::CreateReader(const ReaderConfig& config,
                                  const CallbackFunc<MessageT>& reader_func)
   -> std::shared_ptr<Reader<MessageT>> {
 proto::RoleAttributes role_attr;
 role_attr.set_channel_name(config.channel_name);
 role_attr.mutable_qos_profile()->CopyFrom(config.qos_profile);
 return this->template CreateReader<MessageT>(role_attr, reader_func,
                                              config.pending_queue_size);
}

template <typename MessageT>
auto NodeChannelImpl::CreateReader(const proto::RoleAttributes& role_attr,
                                  const CallbackFunc<MessageT>& reader_func,
                                  uint32_t pending_queue_size)
   -> std::shared_ptr<Reader<MessageT>> {
 if (!role_attr.has_channel_name() || role_attr.channel_name().empty()) {
   AERROR << "Can't create a reader with empty channel name!";
   return nullptr;
 }

 proto::RoleAttributes new_attr(role_attr);
 FillInAttr<MessageT>(&new_attr);

 std::shared_ptr<Reader<MessageT>> reader_ptr = nullptr;
 if (!is_reality_mode_) {
   reader_ptr =
       std::make_shared<blocker::IntraReader<MessageT>>(new_attr, reader_func);
 } else {
   reader_ptr = std::make_shared<Reader<MessageT>>(new_attr, reader_func,
                                                   pending_queue_size);
 }

 RETURN_VAL_IF_NULL(reader_ptr, nullptr);
 RETURN_VAL_IF(!reader_ptr->Init(), nullptr);
 return reader_ptr;
}

template <typename MessageT>
auto NodeChannelImpl::CreateReader(const proto::RoleAttributes& role_attr)
   -> std::shared_ptr<Reader<MessageT>> {
 return this->template CreateReader<MessageT>(role_attr, nullptr);
}

template <typename MessageT>
void NodeChannelImpl::FillInAttr(proto::RoleAttributes* attr) {
 attr->set_host_name(node_attr_.host_name());
 attr->set_host_ip(node_attr_.host_ip());
 attr->set_process_id(node_attr_.process_id());
 attr->set_node_name(node_attr_.node_name());
 attr->set_node_id(node_attr_.node_id());
 auto channel_id = GlobalData::RegisterChannel(attr->channel_name());
 attr->set_channel_id(channel_id);
 if (!attr->has_message_type()) {
   attr->set_message_type(message::MessageType<MessageT>());
 }
 if (!attr->has_proto_desc()) {
   std::string proto_desc("");
   message::GetDescriptorString<MessageT>(attr->message_type(), &proto_desc);
   attr->set_proto_desc(proto_desc);
 }
}

}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_NODE_NODE_CHANNEL_IMPL_H_
