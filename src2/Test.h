#ifndef TEST_H
#define TEST_H

#include <Components/Component.h>
#include <Entities/Entity.h>
#include <Messages/MessageBus.h>
#include <Messages/Messager.h>
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

class MyService : public Services::Service<MyServiceMessage> {
   public:
    static const MyService& Get() {
        static MyService s;
        return s;
    }

   private:
    MyService() {
        attachSubscription(Messages::GetMessageBus().subscribe<MyMessage>(
            [this](const MyMessage& m) {
                mCnt += m.getCount();
                auto msg = std::make_unique<MyMessage>(
                    id(), MyServiceMessage::PrintCount);
                msg->setCount(mCnt);
                Messages::GetMessageBus().queueMessage(std::move(msg));
            },
            id(), id(), MyServiceMessage::IncreaseCount));
    }

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
            MyService::Get()));
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
    MyEntity() {
        addComponent<Components::ComponentManager<Components::Component>>();
        addComponent<MyComponentManager>();
        attachSubscription(Messages::GetMessageBus().subscribe(
            [](const Messages::Message& m) {
                MyService::Get().sendMessage(MyServiceMessage::World);
            },
            id(), MyService::Get(), MyServiceMessage::Hello));
    }
};

#endif
