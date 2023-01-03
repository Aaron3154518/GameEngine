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
KeyboardObservablePtr GetKeyboardObservable() {
    return ServiceSystem::Get<EventService, KeyboardObservable>();
}
TypingObservablePtr GetTypingObservable() {
    return ServiceSystem::Get<EventService, TypingObservable>();
}
}  // namespace EventServices
