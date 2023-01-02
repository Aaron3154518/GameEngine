#include "EventService.h"

namespace EventServices {
EventObservablePtr GetEventObservable() {
    return ServiceSystem::Get<EventService, EventObservable>();
}
ResizeObservablePtr GetResizeObservable() {
    return ServiceSystem::Get<EventService, ResizeObservable>();
}
MouseObservablePtr GetMouseObservable() {
    return ServiceSystem::Get<EventService, MouseObservable>();
}
DragObservablePtr GetDragObservable() {
    return ServiceSystem::Get<EventService, DragObservable>();
}
HoverObservablePtr GetHoverObservable() {
    return ServiceSystem::Get<EventService, HoverObservable>();
}
ScrollObservablePtr GetScrollObservable() {
    return ServiceSystem::Get<EventService, ScrollObservable>();
}
KeyStateObservablePtr GetKeyStateObservable() {
    return ServiceSystem::Get<EventService, KeyStateObservable>();
}
KeyPressObservablePtr GetKeyPressObservable() {
    return ServiceSystem::Get<EventService, KeyPressObservable>();
}
TypingObservablePtr GetTypingObservable() {
    return ServiceSystem::Get<EventService, TypingObservable>();
}
}  // namespace EventServices
