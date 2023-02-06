#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include <Entities/Entity.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Messages/MessageBus.h>

#include <vector>

class TimerService : public Entities::Entity {
   public:
    MESSAGE(TimerMessage, Finished);

    Entities::UUID startTimer(int len_ms);

   private:
    void init();

    void onUpdate(Time dt);

    struct Timer {
        Entities::UUID id;
        int timer_ms = 0;
    };

    std::vector<Timer> mTimers;
};

#endif