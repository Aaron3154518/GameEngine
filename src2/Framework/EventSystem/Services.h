#ifndef EVENT_SERVICES_H
#define EVENT_SERVICES_H

#include <Framework/FrameworkComponents.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>
#include <Utils/Time.h>

class UpdateService : public Services::Service {
   public:
    DATA_MESSAGE(Message, Time, Update);
};

class PhysicsService : public Services::Service {
   public:
   private:
    void service_init();

    void onUpdate(Time dt);
};

#endif