/*
Cyber RT ֧������ͨ�ŷ�ʽ��

����������ͨ�ŷ�ʽ��Publish-Subscribe����Ҳ�л����ŵ���ͨ�ŷ�ʽ�������������������нڵ��ͨ�š��������ߣ�Publisher�����ŵ���channel��ROS �ж�Ӧ�س�Ϊ topic���Ϸ�����Ϣ����Ӧ�Ķ����ߣ�Subscriber������յ���Ϣ����
���񡪿ͻ�ͨ�ŷ�ʽ��Service-Client����Ҳ�л��ڷ����ͨ�ŷ�ʽ��������˫��ġ���Ҫ�������Ӧ���ͨ�ų�����

Ŀǰֻ���ؿ���һ��
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
