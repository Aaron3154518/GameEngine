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

class Fireball : public Entities::Entity {
   public:
    static const Entities::UUID CID;

    void update(Time dt);

    void launch(SDL_FPoint from, float v, const Observables::Node<Rect>& to);

   private:
    void init();
};

class FireballList : public Entities::EntityContainer<Fireball> {
   public:
   private:
    void container_init();
};

class FireballListComponent : public Components::ComponentManager<
                                  Components::DataPtrComponent<FireballList>> {
};

class Wizard : public Entities::Entity {
   public:
   private:
    void init();

    void shootFireball();
};

class Crystal : public Entities::Entity {
   public:
    static const Entities::UUID CID;

   private:
    void init();
};

class Enemy : public Entities::Entity {
    friend class EnemyHandler;

   public:
   private:
    void init();
};

class EnemyHandler : public Entities::EntityContainer<Enemy> {
   public:
   private:
    void container_init();

    void spawnEnemy();
};

class Boundary : public Entities::Entity {
   public:
   private:
    void init();
};

#endif
