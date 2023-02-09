#ifndef TEST_H
#define TEST_H

#include <Components/Component.h>
#include <Entities/Entity.h>
#include <Entities/EntityContainer.h>
#include <Framework/PhysicsSystem/Collision.h>
#include <Framework/PhysicsSystem/Services.h>
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
    void manager_init();

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

extern Rect BOUND;

class MyEntity : public Entities::Entity {
   public:
    enum { Player = 0 };

   private:
    int hp = 5;

    void init();
};

class EnemyProj : public Entities::Entity {
   private:
    void init();
};

typedef Entities::EntityContainer<EnemyProj> EnemyProjCont;

class Enemy : public Entities::Entity {
   private:
    int timer = 1500;
    std::unique_ptr<EnemyProjCont> mProjs;

    void init() {
        mProjs = GameObjects::New<EnemyProjCont>();

        std::mt19937 gen = std::mt19937(rand());
        std::uniform_real_distribution<float> rDist;

        addComponent<PositionComponent>(
            Rect(rDist(gen) * 450, rDist(gen) * 450, 50, 50));
        addComponent<VelocityComponent>(SDL_FPoint{0, 0});
        addComponent<AccelerationComponent>(SDL_FPoint{0, 0});
        addComponent<BoundaryComponent>(BOUND);
        addComponent<PhysicsService>();

        addComponent<ElevationComponent>(0);
        addComponent<SpriteComponent>("res/wizards/crystal.png");
        addComponent<RenderService>();

        addComponent<CollisionComponent>(E);
        addComponent<CollisionService>();

        const float V = rDist(gen) * 50;
        subscribeTo<EventSystem::UpdateMessage>(
            [this, V](const auto& m) {
                auto& v = getComponent<VelocityComponent>().get();
                auto& pos = getComponent<PositionComponent>().get();
                auto& player =
                    GameObjects::Get<PositionComponent>()
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

        subscribeTo<EventSystem::UpdateMessage>(
            [this](const EventSystem::UpdateMessage& m) {
                timer -= m.data.ms();
                if (timer <= 0) {
                    timer += 1500;
                    mProjs->add();

                    auto c = getComponent<PositionComponent>().get().getPos(
                        Rect::Align::CENTER);
                    auto t =
                        GameObjects::Get<PositionComponent>()
                            [GameObjects::Get<MyEntity, MyEntity::Player>()]
                                .get()
                                .getPos(Rect::Align::CENTER);
                    SDL_FPoint v{t.x - c.x, t.y - c.y};
                    float mag = sqrtf(v.x * v.x + v.y * v.y) / 150;
                    if (mag == 0) {
                        mag = 1;
                    }
                    GameObjects::Get<PositionComponent>()[mProjs->back()->id()]
                        .get()
                        .setPos(c.x, c.y, Rect::Align::CENTER);
                    GameObjects::Get<VelocityComponent>()[mProjs->back()->id()]
                        .set({v.x / mag, v.y / mag});
                }
            },
            EventSystem::Update);
    }
};

#endif
