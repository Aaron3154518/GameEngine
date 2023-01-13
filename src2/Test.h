#ifndef TEST_H
#define TEST_H

#include <Components/Component.h>
#include <Entities/Entity.h>
#include <Messages/MessageBus.h>
#include <Messages/MessageReceiver.h>
#include <Messages/MessageSender.h>
#include <Services/CommandService.h>

class MyMessage : public Messages::Message {
   public:
    using Messages::Message::Message;

    void setCount(int cnt) { mCnt = cnt; }
    int getCount() const { return mCnt; }

   private:
    int mCnt = 0;
};

enum MyServiceMessage : Messages::EnumT {
    Hello = 0,
    World,
    IncreaseCount,
    PrintCount
};

class MyService : public Messages::MessageSender<MyServiceMessage>,
                  public Messages::MessageReceiver {
   public:
    MyService() {
        attachSubscription(Messages::GetMessageBus().subscribe(
            [this](const Messages::Message& m) {
                const MyMessage& mm = static_cast<const MyMessage&>(m);
                mCnt += mm.getCount();
                auto msg = std::make_unique<MyMessage>(
                    getType(), MyServiceMessage::PrintCount);
                msg->setCount(mCnt);
                Messages::GetMessageBus().queueMessage(std::move(msg));
            },
            id(), getType(), MyServiceMessage::IncreaseCount));
    }
    MyService(bool) {}

   private:
    int mCnt = 0;
};

class MyComponent : public Components::Component {
   public:
    void onHello() { std::cerr << "Hello" << std::endl; }
    void onWorld(int num) { std::cerr << "World " << num << std::endl; }
    void onCount(int cnt) {
        std::cerr << "Current Count: " << cnt << std::endl;
    }
};

class MyComponentManager : public Components::ComponentManager<MyComponent> {
   public:
    MyComponentManager() {
        attachSubscription(Messages::GetMessageBus().subscribe(
            [this](const Messages::Message& m) { onMyServiceMessage(m); }, id(),
            MyService(false).getType()));
    }

    void onMyServiceMessage(const Messages::Message& m) {
        switch (m.code()) {
            case MyServiceMessage::Hello:
                forEach([](MyComponent& c) { c.onHello(); });
                break;
            case MyServiceMessage::World:
                forEach(&MyComponent::onWorld, 8008135);
                break;
            case MyServiceMessage::PrintCount: {
                const MyMessage& msg = static_cast<const MyMessage&>(m);
                forEach(&MyComponent::onCount, msg.getCount());
            } break;
            default:
                break;
        };
    }
};

class MyEntity : public Entities::Entity {
   public:
    MyEntity(const MyService& s) {
        addComponent<Components::ComponentManager<Components::Component>>();
        addComponent<MyComponentManager>();
        attachSubscription(Messages::GetMessageBus().subscribe(
            [&s](const Messages::Message& m) {
                s.sendMessage(MyServiceMessage::World);
            },
            id(), s.getType(), MyServiceMessage::Hello));
    }
};

#endif
