#ifndef TEST_H
#define TEST_H

#include <Components/Component.h>
#include <Components/NameComponent.h>
#include <Entities/Entity.h>
#include <Messages/GameObjects.h>
#include <Messages/MessageBus.h>
#include <Messages/Messager.h>
#include <Services/CommandService.h>

enum MyServiceMessage : Messages::EnumT {
    Hello = 0,
    World,
    IncreaseCount,
    PrintCount
};

class MyService : public Services::Service {
   private:
    void init();

    void onCommandMessage(const Services::CommandMessage& m);

    int mCnt = 0;
};

using namespace Messages;
typedef Message<MyService, MyServiceMessage> MyMessage;
struct MyCountMessage : public MyMessage {
    MyCountMessage(const MessageData& msg, int cnt)
        : MyMessage(msg), count(cnt) {}

    int count = 0;
};

typedef std::unique_ptr<MyMessage> MyMessagePtr;
typedef std::unique_ptr<MyCountMessage> MyCountMessagePtr;

class MyComponent : public Components::Component {
   public:
    void onHello() { std::cerr << "Hello" << std::endl; }
    void onWorld(int num) { std::cerr << "World " << num << std::endl; }
    void onCount(int cnt) {
        std::cerr << "Current Count: " << cnt << std::endl;
    }
};

class MyComponentManager : public Components::ComponentManager<MyComponent> {
   private:
    void init() {
        Components::ComponentManager<MyComponent>::init();

        attachSubscription(Messages::GetMessageBus().subscribe(
            [this](const Messages::BaseMessage& m) { onMyServiceMessage(m); },
            id(), GameObjects::Get<MyService>()));
        attachSubscription(Messages::GetMessageBus().subscribe<MyCountMessage>(
            [this](const MyCountMessage& m) { onMyServicePrintCount(m); }, id(),
            GameObjects::Get<MyService>(), MyServiceMessage::PrintCount));
    }

    void onMyServiceMessage(const Messages::BaseMessage& m) {
        switch (m.data.code) {
            case MyServiceMessage::Hello:
                forEach([](MyComponent& c) { c.onHello(); });
                break;
            case MyServiceMessage::World:
                forEach(MyComponent::onWorld, 8008135);
                break;
            default:
                break;
        };
    }

    void onMyServicePrintCount(const MyCountMessage& m) {
        int cnt = m.count;
        forEach(MyComponent::onCount, m.count);
        forEach(MyComponent::onCount, cnt);
    }
};

class MyEntity : public Entities::Entity {
   private:
    void init() {
        setName("MyEntity");
        addComponent<Components::ComponentManager<Components::Component>>();
        addComponent<MyComponentManager>();
        attachSubscription(Messages::GetMessageBus().subscribe(
            [](const Messages::BaseMessage& m) {
                Messages::GetMessageBus().queueMessage<MyMessage>(
                    MyServiceMessage::World);
            },
            id(), GameObjects::Get<MyService>(), MyServiceMessage::Hello));
    }
};

typedef std::unique_ptr<MyEntity> MyEntityPtr;

#endif
