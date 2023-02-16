#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <Messages/MessageBus.h>
#include <SDL.h>
#include <Utils/Event.h>
#include <Utils/Time.h>

#include <functional>
#include <vector>

class EventSystem {
   public:
    DATA_MESSAGE(UpdateMessage, Time, Update);
    typedef Messages::Message<Event::Status, Event::KeyButton> KeyboardMessage;
    typedef Messages::Message<Event::Status, Event::MouseButton> MouseMessage;
    struct DragData {
        float x, y, dx, dy;
    };
    DATA_MESSAGE(DragMessage, DragData, DragStart, DragEnd, Dragging);

    typedef std::function<void()> Func;

    EventSystem() = delete;

    static const Event& get();

    static void update();

    static void runNextFrame(const Func& func);
    static void runQueued();

   private:
    static Event mEvent;

    static std::vector<Func>& GetQueued();
};

#endif