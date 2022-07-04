#include "EventService.h"

void EventObservable::next(const Event &e) {
    ServiceSystem::Get<RenderService, RenderOrderObservable>()->computeUnderMouse(e);
    EventObservableBase::next(e);
}