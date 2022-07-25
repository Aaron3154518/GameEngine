#include "EventService.h"

void EventObservable::next(const Event &e) {
    ServiceSystem::Get<RenderService, RenderOrderObservable>()
        ->computeUnderMouse(e);
    for (auto sub : *this) {
        call<0>(*sub, e);
    }
}