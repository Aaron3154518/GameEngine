#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <Messages/MessageBus.h>
#include <Utils/Event.h>
#include <Utils/Time.h>

class EventSystem {
   public:
    DATA_MESSAGE(UpdateMessage, Time, Update);
    typedef Messages::Message<SDL_KeyCode, Event::KeyButton> KeyboardMessage;

    EventSystem() = delete;

    static const Event& get();

    static void update();

   private:
    static Event mEvent;
};

#endif