#ifndef EVENT_SERVICES_H
#define EVENT_SERVICES_H

#include <Framework/EventSystem/EventSystem.h>
#include <Framework/FrameworkComponents.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>
#include <Utils/Time.h>

class PhysicsService : public Services::Service {
   public:
   private:
    void service_init();

    void onUpdate(Time dt);
};

#endif