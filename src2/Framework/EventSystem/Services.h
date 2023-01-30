#ifndef EVENT_SERVICES_H
#define EVENT_SERVICES_H

#include <Framework/EventSystem/EventSystem.h>
#include <Framework/FrameworkComponents.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>
#include <Utils/Time.h>

class VelComponent : public Components::DataComponent<SDL_FPoint> {
   public:
    using Components::DataComponent<SDL_FPoint>::DataComponent;
};
typedef Components::ComponentManager<VelComponent> VelComponentManager;

class AccelComponent : public Components::DataComponent<SDL_FPoint> {
   public:
    using Components::DataComponent<SDL_FPoint>::DataComponent;
};
typedef Components::ComponentManager<AccelComponent> AccelComponentManager;

class BoundaryComponent : public Components::DataComponent<Rect> {
   public:
    using Components::DataComponent<Rect>::DataComponent;
};
typedef Components::ComponentManager<BoundaryComponent>
    BoundaryComponentManager;

class PhysicsService : public Services::Service {
   public:
   private:
    void service_init();

    void onUpdate(Time dt);
};

#endif