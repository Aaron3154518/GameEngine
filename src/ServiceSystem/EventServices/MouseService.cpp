#include "MouseService.h"

// MouseObservable
void MouseObservable::init() {
    eventSub = ServiceSystem::Get<EventService, EventObservable>()->subscribe(
        std::bind(&MouseObservable::onEvent, this, std::placeholders::_1));
    eventSub->setUnsubscriber(unsub);
}

MouseObservable::SubscriptionPtr MouseObservable::subscribe(Subscription::Function func, UIComponentPtr data) {
    SubscriptionPtr retVal = MouseObservableBase::subscribe(func, data);
    ServiceSystem::Get<RenderService>()->addComponent(data);
    return retVal;
}

void MouseObservable::updateSubscriptionData(SubscriptionPtr sub, UIComponentPtr data) {
    ServiceSystem::Get<RenderService>()->removeComponent(sub->getData());
    MouseObservableBase::updateSubscriptionData(sub, data);
    ServiceSystem::Get<RenderService>()->addComponent(data);
}

bool MouseObservable::unsubscribe(SubscriptionPtr sub) {
    ServiceSystem::Get<RenderService>()->removeComponent(sub->getData());
    return true;
}

void MouseObservable::serve(Event::MouseButton mouse) {
    // If a lock exists, nobody gets clicked
    bool locked = mLocks.size() > 0;

    auto underMouse = ServiceSystem::Get<RenderService, RenderOrderObservable>()->getUnderMouse();

    for (SubscriptionPtr sub : mSubscriptions) {
        if (!sub->getData()->visible) {
            continue;
        }

        (*sub)(mouse, !locked && sub->getData() == underMouse);
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
    auto it = std::find_if(mLocks.begin(), mLocks.end(), [lock](const std::unique_ptr<bool> &ptr) -> bool {
        return ptr.get() == lock;
    });
    if (it != mLocks.end()) {
        mLocks.erase(it);
    }
    lock = NULL;
}