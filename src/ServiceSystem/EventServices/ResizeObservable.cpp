#include "ResizeObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
// ResizeObservable
void ResizeObservable::init() {
    mEventSub =
        GetEventObservable()->subscribe([this](const Event& e) { onEvent(e); });
}

void ResizeObservable::onEvent(const Event& e) {
    if (e.resized()) {
        next({e.oldW(), e.oldH(), e.newW(), e.newH()});
    }
}
}  // namespace EventServices