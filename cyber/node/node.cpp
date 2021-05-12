#include "node.h"

#include "../common/global_data.h"
#include "../time/time.h"

namespace apollo {
namespace cyber {

using proto::RoleType;

Node::Node(const std::string& node_name, const std::string& name_space)
    : node_name_(node_name), name_space_(name_space) {
 node_channel_impl_.reset(new NodeChannelImpl(node_name));
}

Node::~Node() {}

const std::string& Node::Name() const { return node_name_; }

void Node::Observe() {
  for (auto& reader : readers_) {
    reader.second->Observe();
  }
}

void Node::ClearData() {
  for (auto& reader : readers_) {
    reader.second->ClearData();
  }
}

}  // namespace cyber
}  // namespace apollo
