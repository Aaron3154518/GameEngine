#include "ResizeService.h"

// ResizeObservable
void ResizeObservable::init() {
    mEventSub = ServiceSystem::Get<EventService, EventObservable>()->subscribe(
        std::bind(&ResizeObservable::onEvent, this, std::placeholders::_1));
}

void ResizeObservable::onEvent(const Event& e) {
    if (e.resized()) {
        next(e.newDim());
    }
}