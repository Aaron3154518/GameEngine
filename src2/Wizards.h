#ifndef WIZARDS_H
#define WIZARDS_H

#include <Entities/EntitySystem.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/PhysicsSystem/Services.h>
#include <Framework/RenderSystem/Services.h>
#include <Framework/RenderSystem/SpriteComponent.h>
#include <Observables/Observables.h>
#include <Utils/Rect.h>

namespace Wizards {
enum _ : Observables::EnumT { Wizard = 0, Crystal };
}
ROOT_NODE(WizPos, Rect, Wizards::_);

class Wizard : public Entities::Entity {
   public:
   private:
    void init();
};

class Crystal : public Entities::Entity {
   public:
   private:
    void init();
};

class Boundary : public Entities::Entity {
   public:
   private:
    void init();
};

#endif
