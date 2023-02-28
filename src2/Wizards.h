#ifndef WIZARDS_H
#define WIZARDS_H

#include <Entities/EntitySystem.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/PhysicsSystem/Collision.h>
#include <Framework/PhysicsSystem/Services.h>
#include <Framework/RenderSystem/Services.h>
#include <Framework/RenderSystem/SpriteComponent.h>
#include <Observables/Observables.h>
#include <Utils/Rect.h>

namespace Wizards {
enum _ : Observables::EnumT { Wizard = 0, Crystal };
}
ROOT_NODE(WizPos, Rect, Wizards::_);

class Fireball : public Entities::Entity {
   public:
    static const Entities::UUID CID;

    void update(Time dt);

   private:
    void init();
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

class Boundary : public Entities::Entity {
   public:
   private:
    void init();
};

#endif
