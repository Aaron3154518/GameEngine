#include "MouseObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
// MouseObservable
void MouseObservable::init() {
    eventSub =
        GetEventObservable()->subscribe([this](const Event& e) { onEvent(e); });
}

void MouseObservable::onSubscribe(SubscriptionPtr sub) {
    ServiceSystem::Get<RenderService, RenderOrderObservable>()->addComponent(
        sub);
}

void MouseObservable::next(Event::MouseButton mouse) {
    // If a lock exists, nobody gets clicked
    bool locked = isLocked();

    auto underMouse = ServiceSystem::Get<RenderService, RenderOrderObservable>()
                          ->getUnderMouse();

    for (auto sub : *this) {
        auto data = sub->get<DATA>();

        if (!data->visible || !data->mouse) {
            continue;
        }

        sub->get<FUNC>()(mouse, !locked && data == underMouse);
    }
}

void MouseObservable::onEvent(Event e) {
    if (e[Event::LEFT].clicked()) {
        next(e[Event::LEFT]);
    }
}
}  // namespace EventServices
