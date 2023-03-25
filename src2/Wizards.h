#ifndef WIZARDS_H
#define WIZARDS_H

#include <Entities/EntitySystem.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/PhysicsSystem/Collision.h>
#include <Framework/PhysicsSystem/Services.h>
#include <Framework/RenderSystem/Services.h>
#include <Framework/RenderSystem/SpriteComponent.h>
#include <Framework/RenderSystem/TextComponent.h>
#include <Observables/Observables.h>
#include <Utils/Number.h>
#include <Utils/Rect.h>

namespace Wizards {
enum _1 : Observables::EnumT { Wizard = 0, Crystal };
enum _2 : Observables::EnumT { Magic = 0 };
enum _3 : Observables::EnumT { InCircle = 0 };
}  // namespace Wizards
ROOT_NODE(WizPos, Rect, Wizards::_1);
ROOT_NODE(CrystalNumbers, Number, Wizards::_2);
ROOT_NODE(WizState, bool, Wizards::_3);

class TargetComponent
    : public Components::ComponentManager<Components::DataComponent<Rect>> {};

void track_ai(const Rect& pos, const Rect& tPos, PhysicsData& pd);
void beeline_ai(const Rect& pos, const Rect& tPos, PhysicsData& pd);

class Fireball : public Entities::Entity, public Entities::EntityContainee {
   public:
    enum AI : uint8_t { Track = 0, Beeline };

    static const Entities::UUID CID;

    using Entities::EntityContainee::EntityContainee;

    void update(Time dt);

    void launch(SDL_FPoint from, float v, const Rect& to, AI ai);
    void launch(SDL_FPoint from, float v, const Observables::Node<Rect>& to,
                AI ai);

   private:
    void init();

    AI mAi = AI::Track;
};

class FireballList : public Entities::EntityContainer<Fireball> {
   public:
   private:
    void init();
};

class FireballListComponent : public Components::ComponentManager<
                                  Components::DataPtrComponent<FireballList>> {
};

class Wizard : public Entities::Entity {
   public:
    static const Entities::UUID CID;

   private:
    void init();

    void shootFireball();
};

class Crystal : public Entities::Entity {
   public:
    static const Entities::UUID CID;
    static const int RAD;

   private:
    void init();
};

class Enemy : public Entities::Entity, public Entities::EntityContainee {
    friend class EnemyHandler;

   public:
    static const Entities::UUID CID;

    using Entities::EntityContainee::EntityContainee;

   private:
    void init();
};

class EnemyHandler : public Entities::EntityContainer<Enemy> {
   public:
    Rect getClosest(const Rect& r);

   private:
    void init();

    void spawnEnemy();
};

class Boundary : public Entities::Entity {
   public:
   private:
    void init();
};

#endif
