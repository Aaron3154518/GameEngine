#ifndef TEST_H
#define TEST_H

#include <Components/Component.h>
#include <Components/NameComponent.h>
#include <Entities/Entity.h>
#include <Messages/GameObjects.h>
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

class MyService : public Services::Service {
   private:
    void init() {
        setName("MyService");
        attachSubscription(Messages::GetMessageBus().subscribe<MyMessage>(
            [this](const MyMessage& m) {
                mCnt += m.getCount();
                auto msg = std::make_unique<MyMessage>(
                    id(), MyServiceMessage::PrintCount);
                msg->setCount(mCnt);
                Messages::GetMessageBus().queueMessage(std::move(msg));
            },
            id(), id(), MyServiceMessage::IncreaseCount));
        attachSubscription(
            Messages::GetMessageBus().subscribe<Services::CommandMessage>(
                [this](const Services::CommandMessage& m) {
                    onCommandMessage(m);
                },
                id(), GameObjects::Get<Services::CommandService>(),
                Services::CommandMessage::Command));
    }

    void onCommandMessage(const Services::CommandMessage& m) {
        auto code = m.cmdCode();
        switch (code) {
            case MyServiceMessage::Hello:
            case MyServiceMessage::World:
                Messages::GetMessageBus().queueMessage(
                    std::make_unique<Messages::Message>(id(), code));
                break;
            case MyServiceMessage::PrintCount:
            case MyServiceMessage::IncreaseCount: {
                std::stringstream ss(m.line());
                int val = 0;
                if (code == MyServiceMessage::IncreaseCount) {
                    if (!(ss >> val)) {
                        val = 1;
                    }
                }
                auto m = std::make_unique<MyMessage>(
                    id(), MyServiceMessage::IncreaseCount);
                m->setCount(val);
                Messages::GetMessageBus().queueMessage(std::move(m));
            } break;
            default:
                break;
        }
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
   private:
    void init() {
        Components::ComponentManager<MyComponent>::init();

        attachSubscription(Messages::GetMessageBus().subscribe(
            [this](const Messages::Message& m) { onMyServiceMessage(m); }, id(),
            GameObjects::Get<MyService>()));
        attachSubscription(Messages::GetMessageBus().subscribe<MyMessage>(
            [this](const MyMessage& m) { onMyServicePrintCount(m); }, id(),
            GameObjects::Get<MyService>(), MyServiceMessage::PrintCount));
    }

    void onMyServiceMessage(const Messages::Message& m) {
        switch (m.code()) {
            case MyServiceMessage::Hello:
                forEach([](MyComponent& c) { c.onHello(); });
                break;
            case MyServiceMessage::World:
                forEach(&MyComponent::onWorld, 8008135);
                break;
            default:
                break;
        };
    }

    void onMyServicePrintCount(const MyMessage& m) {
        forEach(&MyComponent::onCount, m.getCount());
    }
};

class MyEntity : public Entities::Entity {
   private:
    void init() {
        setName("MyEntity");
        addComponent<Components::ComponentManager<Components::Component>>();
        addComponent<MyComponentManager>();
        attachSubscription(Messages::GetMessageBus().subscribe(
            [](const Messages::Message& m) {
                Messages::GetMessageBus().queueMessage(
                    std::make_unique<MyMessage>(GameObjects::Get<MyService>(),
                                                MyServiceMessage::World));
            },
            id(), GameObjects::Get<MyService>(), MyServiceMessage::Hello));
    }
};

typedef std::unique_ptr<MyEntity> MyEntityPtr;

#endif
