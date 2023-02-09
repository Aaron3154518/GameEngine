#ifndef EVENT_SERVICES_H
#define EVENT_SERVICES_H

#include <Entities/Entity.h>
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

class PhysicsService : public Services::Service {
   private:
    void manager_init();

    void onUpdate(Time dt);
};

#endif