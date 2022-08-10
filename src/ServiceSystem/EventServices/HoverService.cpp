#include "HoverService.h"

// HoverObservable
void HoverObservable::init() {
    mEventSub = ServiceSystem::Get<EventService, EventObservable>()->subscribe(
        std::bind(&HoverObservable::onEvent, this, std::placeholders::_1));
}

void HoverObservable::onSubscribe(SubscriptionPtr sub) {
    ServiceSystem::Get<RenderService, RenderOrderObservable>()->addComponent(
        sub);
}

void HoverObservable::next(SDL_Point mouse) {
    // If a lock exists, nobody gets hovered
    if (ServiceSystem::Get<MouseService, MouseObservable>()->isLocked()) {
        SubscriptionPtr currHover = mCurrHover.lock();
        if (currHover) {
            currHover->get<ON_MOUSE_LEAVE>()();
            mCurrHover.reset();
        }
        return;
    }

    auto underHover = ServiceSystem::Get<RenderService, RenderOrderObservable>()
                          ->getUnderMouse();

    bool found = false;
    for (auto sub : *this) {
        auto data = sub->get<DATA>();

        if (!data->visible) {
            continue;
        }

        if (data == underHover) {
            SubscriptionPtr currHover = mCurrHover.lock();
            if (currHover != sub) {
                if (currHover) {
                    currHover->get<ON_MOUSE_LEAVE>()();
                }
                sub->get<ON_MOUSE_ENTER>()();
                mCurrHover = sub;
            }
            sub->get<ON_HOVER>()(mouse);
            found = true;
            break;
        }
    }

    if (!found) {
        SubscriptionPtr currHover = mCurrHover.lock();
        if (currHover) {
            currHover->get<ON_MOUSE_LEAVE>()();
            mCurrHover.reset();
        }
    }
}

void HoverObservable::onEvent(Event e) { next(e.mouse()); }
