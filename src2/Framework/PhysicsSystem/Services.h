#ifndef EVENT_SERVICES_H
#define EVENT_SERVICES_H

#include <Framework/EventSystem/EventSystem.h>
#include <Framework/FrameworkComponents.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>
#include <Utils/Time.h>

class VelocityComponent : public Components::ComponentManager<
                              Components::DataComponent<SDL_FPoint>> {};

class AccelerationComponent : public Components::ComponentManager<
                                  Components::DataComponent<SDL_FPoint>> {};

class BoundaryComponent
    : public Components::ComponentManager<Components::DataComponent<Rect>> {};

SERVICE(PhysicsService, PositionComponent, pos, VelocityComponent, v,
        AccelerationComponent, a, BoundaryComponent, bounds) {
   private:
    void manager_init();

    void onUpdate(Time dt);
};

SERVICE2(PhysicsService3, PositionComponent, VelocityComponent,
         AccelerationComponent, BoundaryComponent);
class PhysicsService2 : public PhysicsService3 {
   public:
   private:
    void manager_init();
};

#endif