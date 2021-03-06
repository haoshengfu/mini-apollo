#include "component.h"

#include <gtest/gtest.h>
#include <memory>

#include "../init.h"
#include "../message/raw_message.h"

namespace apollo {
namespace cyber {

using apollo::cyber::Component;
using apollo::cyber::message::RawMessage;
using apollo::cyber::proto::ComponentConfig;
using apollo::cyber::proto::TimerComponentConfig;
static bool ret_proc = true;
static bool ret_init = true;
template <typename M0, typename M1 = NullType, typename M2 = NullType,
          typename M3 = NullType>
class Component_A : public Component<M0, M1, M2, M3> {
 public:
  Component_A() {}
  bool Init() { return ret_init; }

 private:
  bool Proc(const std::shared_ptr<M0> &msg0, const std::shared_ptr<M1> &msg1,
            const std::shared_ptr<M2> &msg2, const std::shared_ptr<M3> &msg3) {
    return ret_proc;
  }
};

template <typename M0, typename M1>
class Component_B : public Component<M0, M1> {
 public:
  Component_B() {}
  bool Init() { return ret_init; }

 private:
  bool Proc(const std::shared_ptr<M0> &, const std::shared_ptr<M1> &) {
    return ret_proc;
  }
};

template <typename M0>
class Component_C : public Component<M0> {
 public:
  Component_C() {}
  bool Init() { return ret_init; }

 private:
  bool Proc(const std::shared_ptr<M0> &) { return ret_proc; }
};

TEST(TimerComponent, init) {
  ret_proc = true;
  ret_init = true;
  apollo::cyber::proto::ComponentConfig compcfg;
  auto msg_str1 = std::make_shared<message::RawMessage>();
  auto msg_str2 = std::make_shared<message::RawMessage>();
  auto msg_str3 = std::make_shared<message::RawMessage>();
  auto msg_str4 = std::make_shared<message::RawMessage>();

  compcfg.set_name("perception");
  apollo::cyber::proto::ReaderOption *read_opt = compcfg.add_readers();
  read_opt->set_channel("/perception/channel");
  auto comC = std::make_shared<Component_C<RawMessage>>();
  EXPECT_EQ(true, comC->Initialize(compcfg));
  EXPECT_EQ(true, comC->Process(msg_str1));

  // compcfg.set_name("perception2");
  // apollo::cyber::proto::ReaderOption *read_opt2 = compcfg.add_readers();
  // read_opt2->set_channel("/driver/channel1");
  // auto comB = std::make_shared<Component_B<RawMessage, RawMessage>>();
  // EXPECT_EQ(true, comB->Initialize(compcfg));
  // EXPECT_EQ(true, comB->Process(msg_str1, msg_str2));

  // compcfg.set_name("perception3");
  // apollo::cyber::proto::ReaderOption *read_opt3 = compcfg.add_readers();
  // read_opt3->set_channel("/driver/channel2");
  // compcfg.set_name("perception4");
  // apollo::cyber::proto::ReaderOption *read_opt4 = compcfg.add_readers();
  // read_opt4->set_channel("/driver/channel3");
  // auto comA = std::make_shared<
  //     Component_A<RawMessage, RawMessage, RawMessage, RawMessage>>();
  // EXPECT_EQ(true, comA->Initialize(compcfg));
  // EXPECT_EQ(true, comA->Process(msg_str1, msg_str2, msg_str3, msg_str4));
}

// TEST(TimerComponentFail, init) {
//   ret_proc = false;
//   ret_init = false;
//   apollo::cyber::proto::ComponentConfig compcfg;

//   auto msg_str1 = std::make_shared<message::RawMessage>();
//   auto msg_str2 = std::make_shared<message::RawMessage>();
//   auto msg_str3 = std::make_shared<message::RawMessage>();
//   auto msg_str4 = std::make_shared<message::RawMessage>();

//   compcfg.set_name("perception_f");
//   apollo::cyber::proto::ReaderOption *read_opt = compcfg.add_readers();
//   read_opt->set_channel("/perception/channel");
//   auto comC = std::make_shared<Component_C<RawMessage>>();
//   EXPECT_EQ(false, comC->Initialize(compcfg));
//   EXPECT_EQ(false, comC->Process(msg_str1));

//   compcfg.set_name("perception2_f");
//   apollo::cyber::proto::ReaderOption *read_opt2 = compcfg.add_readers();
//   read_opt2->set_channel("/driver/channel");
//   auto comB = std::make_shared<Component_B<RawMessage, RawMessage>>();
//   EXPECT_EQ(false, comB->Initialize(compcfg));
//   EXPECT_EQ(false, comB->Process(msg_str1, msg_str2));

//   compcfg.set_name("perception3_F");
//   apollo::cyber::proto::ReaderOption *read_opt3 = compcfg.add_readers();
//   read_opt3->set_channel("/driver/channel");
//   auto comA = std::make_shared<
//       Component_A<RawMessage, RawMessage, RawMessage, RawMessage>>();
//   EXPECT_EQ(false, comA->Initialize(compcfg));
//   EXPECT_EQ(false, comA->Process(msg_str1, msg_str2, msg_str3, msg_str4));
// }

}  // namespace cyber
}  // namespace apollo

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  apollo::cyber::Init(argv[0]);
  auto res = RUN_ALL_TESTS();
  return res;
}
