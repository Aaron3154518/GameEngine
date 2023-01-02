#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/EventServices/KeyboardServices/KeyPressObservable.h>
#include <ServiceSystem/EventServices/KeyboardServices/KeyStateObservable.h>
#include <ServiceSystem/EventServices/KeyboardServices/TypingObservable.h>
#include <ServiceSystem/EventServices/MouseServices/DragObservable.h>
#include <ServiceSystem/EventServices/MouseServices/HoverObservable.h>
#include <ServiceSystem/EventServices/MouseServices/MouseObservable.h>
#include <ServiceSystem/EventServices/MouseServices/ScrollObservable.h>
#include <ServiceSystem/EventServices/ResizeObservable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>

namespace EventServices {
EventObservablePtr GetEventObservable();
ResizeObservablePtr GetResizeObservable();
MouseObservablePtr GetMouseObservable();
DragObservablePtr GetDragObservable();
HoverObservablePtr GetHoverObservable();
ScrollObservablePtr GetScrollObservable();
KeyStateObservablePtr GetKeyStateObservable();
KeyPressObservablePtr GetKeyPressObservable();
TypingObservablePtr GetTypingObservable();

class EventService
    : public Service<EventObservable, ResizeObservable, MouseObservable,
                     DragObservable, HoverObservable, ScrollObservable,
                     KeyPressObservable, KeyStateObservable, TypingObservable> {
};
}  // namespace EventServices

#endif
