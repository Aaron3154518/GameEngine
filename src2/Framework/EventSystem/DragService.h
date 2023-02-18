#ifndef DRAG_SERVICE_H
#define DRAG_SERVICE_H

#include <Framework/EventSystem/EventSystem.h>
#include <Framework/FrameworkComponents.h>
#include <Services/Service.h>
#include <Utils/Event.h>

struct MouseOptions : public Components::Component {
    MouseOptions(bool drag);

    bool canDrag = false;
};

class MouseService : public Services::Service<MouseOptions> {
   public:
    typedef Messages::Message<Event::Status, Event::MouseButton> MouseMessage;
    struct DragData {
        int x = 0, y = 0, dx = 0, dy = 0;
    };
    DATA_MESSAGE(DragMessage, DragData, DragStart, DragEnd, Dragging);

   private:
    void manager_init();

    void onEvent(const Event& e);

    Entities::UUID mDragId;
};

#endif
