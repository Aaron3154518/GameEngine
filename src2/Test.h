#ifndef TEST_H
#define TEST_H

#include <Components/Component.h>
#include <Entities/Entity.h>
#include <Framework/EventSystem/Services.h>
#include <Framework/PhysicsSystem/Collision.h>
#include <Framework/RenderSystem/Services.h>
#include <Messages/GameObjects.h>
#include <Messages/MessageBus.h>
#include <Messages/Messager.h>

#include <cmath>
#include <random>

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

extern Entities::UUID P;
extern Entities::UUID E;

class MyEntity : public Entities::Entity {
   public:
    enum { Player = 0 };

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
        addComponent<BoundaryComponentManager>(Rect(0, 0, 500, 500));
        // addComponent<AccelComponentManager>(SDL_FPoint{0, 25});
        GameObjects::Get<PhysicsService>().subscribe(id());

        addComponent<ElevationComponentManager>(1);
        addComponent<SpriteComponentManager>("res/wizards/wizard_ss.png", 5,
                                             150);
        GameObjects::Get<RenderService>().subscribe(id());

        addComponent<CollisionComponentManager>(P);
        GameObjects::Get<CollisionService>().subscribe(id());
        subscribeTo<CollisionService::Message>(
            [this](const CollisionService::Message& m) {
                getComponent<PositionComponentManager>().get() =
                    Rect(10, 10, 50, 50);
            },
            CollisionService::Collided);

        subscribeTo<EventSystem::KeyboardMessage>(
            [this](const EventSystem::KeyboardMessage& m) {
                auto& pos = getComponent<PositionComponentManager>().get();
                switch (m.data.key) {
                    case SDLK_r:
                        pos = Rect(10, 10, 50, 50);
                        break;
                    default:
                        break;
                }
            },
            Event::PRESSED);
        static int V = 100;
        subscribeTo<EventSystem::KeyboardMessage>(
            [this](const EventSystem::KeyboardMessage& m) {
                auto& v = getComponent<VelComponentManager>().get();
                switch (m.data.key) {
                    case SDLK_a:
                        v.x = -V;
                        break;
                    case SDLK_d:
                        v.x = V;
                        break;
                    case SDLK_w:
                        v.y = -V;
                        break;
                    case SDLK_s:
                        v.y = V;
                        break;
                    default:
                        break;
                }
            },
            Event::HELD);
        subscribeTo<EventSystem::KeyboardMessage>(
            [this](const EventSystem::KeyboardMessage& m) {
                auto& v = getComponent<VelComponentManager>().get();
                switch (m.data.key) {
                    case SDLK_a:
                    case SDLK_d:
                        v.x = 0;
                        break;
                    case SDLK_w:
                    case SDLK_s:
                        v.y = 0;
                        break;
                    default:
                        break;
                }
            },
            Event::RELEASED);
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

class Enemy : public Entities::Entity {
   private:
    void init() {
        std::mt19937 gen = std::mt19937(rand());
        std::uniform_real_distribution<float> rDist;

        addComponent<PositionComponentManager>(
            Rect(rDist(gen) * 450, rDist(gen) * 450, 50, 50));
        addComponent<VelComponentManager>(SDL_FPoint{0, 0});
        addComponent<BoundaryComponentManager>(Rect(0, 0, 500, 500));
        GameObjects::Get<PhysicsService>().subscribe(id());

        addComponent<ElevationComponentManager>(0);
        addComponent<SpriteComponentManager>("res/wizards/crystal.png");
        GameObjects::Get<RenderService>().subscribe(id());

        addComponent<CollisionComponentManager>(E);
        GameObjects::Get<CollisionService>().subscribe(id());

        const float V = rDist(gen) * 50;
        subscribeTo<EventSystem::UpdateMessage>(
            [this, V](const auto& m) {
                auto& v = getComponent<VelComponentManager>().get();
                auto& pos = getComponent<PositionComponentManager>().get();
                auto& player =
                    GameObjects::Get<PositionComponentManager>()
                        [GameObjects::Get<MyEntity, MyEntity::Player>()]
                            .get();
                v.x = player.cX() - pos.cX();
                v.y = player.cY() - pos.cY();
                float mag = sqrtf(v.x * v.x + v.y * v.y);
                if (mag > 0) {
                    v.x *= V / mag;
                    v.y *= V / mag;
                }
            },
            EventSystem::Update);
    }
};

#endif
