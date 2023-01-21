#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <Utils/Event.h>

class EventSystem {
   public:
    EventSystem() = delete;

    static const Event& get();

    static void update();

   private:
    static Event mEvent;
};

#endif