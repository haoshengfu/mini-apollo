/*
Cyber RT 支持两种通信方式：

发布—订阅通信方式（Publish-Subscribe），也叫基于信道的通信方式，常用于数据流处理中节点间通信。即发布者（Publisher）在信道（channel，ROS 中对应地称为 topic）上发布消息，相应的订阅者（Subscriber）便会收到消息数据
服务—客户通信方式（Service-Client），也叫基于服务的通信方式，常用于双向的、需要有请求和应答的通信场景。

目前只着重看第一种
*/


#ifndef CYBER_NODE_NODE_H_
#define CYBER_NODE_NODE_H_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <mutex>
#include "node_channel_impl.h"


namespace apollo {
namespace cyber {

template <typename M0, typename M1, typename M2, typename M3>
class Component;
class TimerComponent;

class Node {
public:
    template <typename M0, typename M1, typename M2, typename M3>
    friend class Component;
    friend class TimerComponent;
    friend std::unique_ptr<Node> CreateNode(const std::string&, const std::string&);


    void Observe();
    void ClearData();
    
    virtual ~Node();
    const std::string& Name() const;

private:
    explicit Node(const std::string& node_name, const std::string& name_space = "");
    std::string node_name_;
    std::string name_space_;
    std::mutex readers_mutex_;
    std::map<std::string, std::shared_ptr<ReaderBase>> readers_;

    std::unique_ptr<NodeChannelImpl> node_channel_impl_ = nullptr;
};


}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_NODE_NODE_H_
