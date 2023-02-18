#ifndef TEST_H
#define TEST_H

#include <Components/Component.h>
#include <Entities/Entity.h>
#include <Entities/EntityContainer.h>
#include <Framework/EventSystem/DragService.h>
#include <Framework/PhysicsSystem/Collision.h>
#include <Framework/PhysicsSystem/Services.h>
#include <Framework/RenderSystem/Services.h>
#include <Framework/RenderSystem/TextComponent.h>
#include <Messages/GameObjects.h>
#include <Messages/MessageBus.h>
#include <Messages/Messager.h>

#include <cmath>
#include <random>

class MyService : public Services::Service<> {
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

class HealthData : public Entities::Entity,
                   public Components::DataComponent<int> {
   public:
    HealthData(int hp, Rect rect);

    void operator++();
    void operator--();
    void operator=(int hp);

   private:
    void init();

    void update();

    const Rect mRect;
    const std::vector<std::string> mImgs;
    int a = 0;
};

class HealthComponent : public Components::ComponentManager<HealthData> {};

class MyEntity : public Entities::Entity {
   public:
    enum { Player = 0 };

   private:
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

    void init();
};

#endif
