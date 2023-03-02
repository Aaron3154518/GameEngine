#ifndef EVENT_SERVICES_H
#define EVENT_SERVICES_H

#include <Entities/Entity.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/FrameworkComponents.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>
#include <Utils/Time.h>

struct PhysicsData : public Components::Component {
    SDL_FPoint v, a;
    float maxV = -1;
};

class BoundaryComponent
    : public Components::ComponentManager<Components::DataComponent<Rect>> {};

class PhysicsService : public Services::Service<PhysicsData> {
   private:
    void manager_init();

    void onUpdate(Time dt);
};

#endif