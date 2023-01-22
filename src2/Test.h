#ifndef TEST_H
#define TEST_H

#include <Components/Component.h>
#include <Components/NameComponent.h>
#include <Entities/Entity.h>
#include <Framework/RenderSystem/Services.h>
#include <Messages/GameObjects.h>
#include <Messages/MessageBus.h>
#include <Messages/Messager.h>
#include <Services/CommandService.h>

class MyService : public Services::Service {
   public:
    enum Code : Messages::EnumT { Hello = 0, World, IncreaseCount, PrintCount };
    typedef Messages::Message<MyService> Message;
    typedef Messages::Message<MyService, int> CountMessage;

   private:
    void service_init();

    void onCommandMessage(const Services::CommandService::CommandMessage& m);

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
    void manager_init() {
        attachSubscription(
            Messages::GetMessageBus().subscribe<MyService::Message>(
                [this](const auto& m) { onMyServiceMessage(m); }, id(),
                GameObjects::Get<MyService>()));
        attachSubscription(
            Messages::GetMessageBus().subscribe<MyService::CountMessage>(
                [this](const auto& m) { onMyServicePrintCount(m); }, id(),
                GameObjects::Get<MyService>(), MyService::Code::PrintCount));
    }

    void onMyServiceMessage(const MyService::Message& m) {
        switch (m.code) {
            case MyService::Code::Hello:
                forEach([](MyComponent& c) { c.onHello(); });
                break;
            case MyService::Code::World:
                forEach(MyComponent::onWorld, 8008135);
                break;
            default:
                break;
        };
    }

    void onMyServicePrintCount(const MyService::CountMessage& m) {
        int cnt = m.data;
        forEach(MyComponent::onCount, m.data);
        forEach(MyComponent::onCount, cnt);
    }
};

class MyEntity : public Entities::Entity {
   private:
    void init() {
        setName("MyEntity");
        addComponent<Components::ComponentManager<Components::Component>>();
        addComponent<MyComponentManager>();
        attachSubscription(
            Messages::GetMessageBus().subscribe<MyService::Message>(
                [](const auto& m) {
                    Messages::GetMessageBus().sendMessage(
                        MyService::Message(MyService::Code::World));
                },
                id(), GameObjects::Get<MyService>(), MyService::Code::Hello));

        addComponent<ElevationComponentManager>(1);
        addComponent<PositionComponentManager>(Rect(10, 10, 50, 50));
        addComponent<SpriteComponentManager>("res/wizards/wizard.png");
        GameObjects::Get<RenderService>().subscribe(id());
    }
};

typedef std::unique_ptr<MyEntity> MyEntityPtr;

#endif
