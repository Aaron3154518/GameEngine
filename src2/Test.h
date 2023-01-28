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
    MESSAGE(Message, Hello, World);
    DATA_MESSAGE(CountMessage, int, IncreaseCount, PrintCount);

    int count() const;

   private:
    void service_init();

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
        subscribeTo<MyService::Message>(
            [this](const auto& m) { onMyServiceMessage(m); });
        subscribeTo<MyService::CountMessage>(
            [this](const auto& m) { onMyServicePrintCount(m); },
            MyService::PrintCount);
    }

    void onMyServiceMessage(const MyService::Message& m) {
        switch (m.code) {
            case MyService::Hello:
                forEach([](MyComponent& c) { c.onHello(); });
                break;
            case MyService::World:
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
        subscribeTo<MyService::Message>(
            [](const auto& m) {
                Messages::GetMessageBus().sendMessage(
                    MyService::Message(MyService::World));
            },
            MyService::Hello);
        addComponent<ElevationComponentManager>(1);
        addComponent<PositionComponentManager>(Rect(10, 10, 50, 50));
        addComponent<SpriteComponentManager>("res/wizards/wizard.png");
        GameObjects::Get<RenderService>().subscribe(id());
    }
};

typedef std::unique_ptr<MyEntity> MyEntityPtr;

#endif
