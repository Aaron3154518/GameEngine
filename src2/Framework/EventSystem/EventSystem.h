#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <Messages/MessageBus.h>
#include <SDL.h>
#include <Utils/Event.h>
#include <Utils/Time.h>

class EventSystem {
   public:
    DATA_MESSAGE(UpdateMessage, Time, Update);
    typedef Messages::Message<Event::Status, Event::KeyButton> KeyboardMessage;
    typedef Messages::Message<Event::Status, Event::MouseButton> MouseMessage;

    EventSystem() = delete;

    static const Event& get();

    static void update();

   private:
    static Event mEvent;
};

#endif