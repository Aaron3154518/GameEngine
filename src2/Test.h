#ifndef TEST_H
#define TEST_H

#include <Components/Component.h>
#include <Entities/Entity.h>
#include <Framework/EventSystem/Services.h>
#include <Framework/RenderSystem/Services.h>
#include <Messages/GameObjects.h>
#include <Messages/MessageBus.h>
#include <Messages/Messager.h>

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
            [this](const MyService::Message& m) { onMyServiceMessage(m); });
        subscribeTo<MyService::CountMessage>(
            [this](const MyService::CountMessage& m) {
                onMyServicePrintCount(m);
            },
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
        addComponent<Components::ComponentManager<Components::Component>>();
        addComponent<MyComponentManager>();
        subscribeTo<MyService::Message>(
            [](const MyService::Message& m) {
                Messages::GetMessageBus().sendMessage(
                    MyService::Message(MyService::World));
            },
            MyService::Hello);

        addComponent<PositionComponentManager>(Rect(10, 10, 50, 50));
        addComponent<VelComponentManager>(SDL_FPoint{0, 0});
        addComponent<AccelComponentManager>(SDL_FPoint{10, 10});
        GameObjects::Get<PhysicsService>().subscribe(id());

        addComponent<ElevationComponentManager>(1);
        addComponent<SpriteComponentManager>("res/wizards/wizard_ss.png");
        GameObjects::Get<RenderService>().subscribe(id());

        addComponent<AnimationComponentManager>(5, 150);
        GameObjects::Get<AnimationService>().subscribe(id());

        subscribeTo<EventSystem::KeyboardMessage>(
            [this](const EventSystem::KeyboardMessage& m) {
                if (m.data.key == SDLK_r) {
                    getComponent<PositionComponentManager>().get() =
                        Rect(10, 10, 50, 50);
                }
            },
            Event::PRESSED);
        subscribeTo<EventSystem::MouseMessage>(
            [this](const EventSystem::MouseMessage& m) {
                if (m.data.mouse == Event::LEFT) {
                    getComponent<PositionComponentManager>().get().setPos(
                        m.data.clickPos.x, m.data.clickPos.y,
                        Rect::Align::CENTER);
                }
            },
            Event::CLICKED);
    }
};

typedef std::unique_ptr<MyEntity> MyEntityPtr;

#endif
