#include "MouseService.h"

// MouseObservable
void MouseObservable::init() {
    eventSub = ServiceSystem::Get<EventService, EventObservable>()->subscribe(
        std::bind(&MouseObservable::onEvent, this, std::placeholders::_1));
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

        if (!data->visible) {
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

void *MouseObservable::requestLock() {
    return mLocks.insert(std::make_unique<bool>()).first->get();
}

void MouseObservable::releaseLock(void *&lock) {
    auto it = std::find_if(mLocks.begin(), mLocks.end(),
                           [lock](const std::unique_ptr<bool> &ptr) -> bool {
                               return ptr.get() == lock;
                           });
    if (it != mLocks.end()) {
        mLocks.erase(it);
    }
    lock = NULL;
}

bool MouseObservable::isLocked() const { return mLocks.size() > 0; }
