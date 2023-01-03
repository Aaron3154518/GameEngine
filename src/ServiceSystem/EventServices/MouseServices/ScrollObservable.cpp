#include "ScrollObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
// ScrollObservable
void ScrollObservable::next(int scroll) {
    auto underMouse = ServiceSystem::Get<RenderService, RenderOrderObservable>()
                          ->getUnderMouse();

    for (auto sub : *this) {
        auto pos = sub->get<POS>();

        if (pos == underMouse) {
            if (pos->visible) {
                sub->get<ON_SCROLL>()(scroll);
            }
            break;
        }
    }
}

void ScrollObservable::init() {
    mEventSub =
        GetEventObservable()->subscribe([this](const Event& e) { onEvent(e); });
}

void ScrollObservable::onEvent(const Event& e) {
    int s = e.scroll();
    if (s != 0) {
        next(s);
    }
}
}  // namespace EventServices