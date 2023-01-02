#include "ResizeService.h"

// ResizeObservable
void ResizeObservable::init() {
    mEventSub = ServiceSystem::Get<EventService, EventObservable>()->subscribe(
        [this](const Event& e) { onEvent(e); });
}

void ResizeObservable::onEvent(const Event& e) {
    if (e.resized()) {
        next({e.oldW(), e.oldH(), e.newW(), e.newH()});
    }
}