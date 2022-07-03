#include "MouseService.h"

// MouseObservable
void MouseObservable::init() {
    eventSub = ServiceSystem::Get<EventService>()->Get<EventObservable>()->subscribe(
        std::bind(&MouseObservable::onEvent, this, std::placeholders::_1));
    eventSub->setUnsubscriber(unsub);
    renderSub = ServiceSystem::Get<RenderService>()->Get<RenderOrderObservable>()->subscribe(
        std::bind(&MouseObservable::onRenderOrder, this, std::placeholders::_1));
    renderSub->setUnsubscriber(unsub);
}

MouseObservable::SubscriptionPtr MouseObservable::subscribe(SubscriptionT::Function func, UIComponentPtr data) {
    SubscriptionPtr retVal =
        Observable<Event::MouseButton, void(Event::MouseButton, bool), UIComponent>::subscribe(func, data);
    ServiceSystem::Get<RenderService>()->addComponent(data);
    return retVal;
}

bool MouseObservable::unsubscribe(SubscriptionPtr sub) {
    ServiceSystem::Get<RenderService>()->removeComponent(sub->data);
    return true;
}

void MouseObservable::serve(Event::MouseButton mouse) {
    // If a lock exists, nobody gets clicked
    bool foundTop = mLocks.size() != 0;
    for (SubscriptionPtr sub : mSubscriptions) {
        if (!sub->data->visible) {
            continue;
        }

        if (!foundTop && SDL_PointInRect(&mouse.clickPos, &sub->data->rect)) {
            (*sub)(mouse, true);
            foundTop = true;
        } else {
            (*sub)(mouse, false);
        }
    }
}

void MouseObservable::onEvent(Event e) {
    if (e[Event::LEFT].clicked()) {
        next(e[Event::LEFT]);
    }
}

void MouseObservable::onRenderOrder(const std::vector<UIComponentPtr> &order) {
    std::unordered_map<SubscriptionPtr, int> idxs;

    // Map out the order position of each subscription
    for (auto sub : mSubscriptions) {
        idxs[sub] = std::find(order.begin(), order.end(), sub->data) - order.begin();
    }

    // Sort the subcription by ascending order position
    mSubscriptions.sort([&idxs](const SubscriptionPtr &a, const SubscriptionPtr &b) -> bool {
        return idxs.at(a) > idxs.at(b);
    });
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