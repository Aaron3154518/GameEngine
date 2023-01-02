#include "MouseObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
const std::function<void(Event::MouseButton, bool)>
    MouseObservable::DO_NOTHING = [](Event::MouseButton b, bool c) {};

// MouseObservable::Lock
MouseObservable::Lock::Lock(const MouseVector& mice) : mMice(mice) {
    for (Event::Mouse m : mMice) {
        mLocks[m];
    }
}

const MouseVector& MouseObservable::Lock::getMice() const { return mMice; }

bool MouseObservable::Lock::isLocked() const {
    for (auto& pair : mLocks) {
        if (pair.second) {
            return true;
        }
    }

    return false;
}

void MouseObservable::Lock::setLock(::Lock l, Event::Mouse mouse) {
    mLocks.at(mouse) = l;
}
Lock& MouseObservable::Lock::getLock(Event::Mouse mouse) {
    return mLocks.at(mouse);
}

// MouseObservable
void MouseObservable::init() {
    eventSub =
        GetEventObservable()->subscribe([this](const Event& e) { onEvent(e); });
}

MouseObservable::SubscriptionPtr MouseObservable::subscribe(
    std::function<void(Event::MouseButton, bool)> onClick, UIComponentPtr pos) {
    return subscribe(onClick, onClick, onClick, pos);
}

MouseObservable::SubscriptionPtr MouseObservable::subscribe(
    std::function<void(Event::MouseButton, bool)> onClick, Event::Mouse btn,
    UIComponentPtr pos) {
    return subscribe(btn == Event::Mouse::LEFT ? onClick : DO_NOTHING,
                     btn == Event::Mouse::RIGHT ? onClick : DO_NOTHING,
                     btn == Event::Mouse::RIGHT ? onClick : DO_NOTHING, pos);
}

void MouseObservable::onSubscribe(SubscriptionPtr sub) {
    ServiceSystem::Get<RenderService, RenderOrderObservable>()->addComponent(
        sub);
}

void MouseObservable::next(Event::MouseButton mouse) {
    // If a lock exists, nobody gets clicked
    bool locked = isLocked(mouse.mouse);

    auto underMouse = ServiceSystem::Get<RenderService, RenderOrderObservable>()
                          ->getUnderMouse();

    for (auto sub : *this) {
        auto data = sub->get<DATA>();

        if (!data->visible || !data->mouse) {
            continue;
        }

        getOnClick(sub, mouse.mouse)(mouse, !locked && data == underMouse);
    }
}

MouseObservable::Lock MouseObservable::requestLock(const MouseVector& mice) {
    Lock l(mice);
    for (auto mouse : mice) {
        l.setLock(mLocks[mouse].requestLock(), mouse);
    }
    return l;
}
void MouseObservable::releaseLock(Lock& lock) {
    for (auto mouse : lock.getMice()) {
        mLocks[mouse].releaseLock(lock.getLock(mouse));
    }
}
bool MouseObservable::isLocked(Event::Mouse mouse) const {
    return mLocks[mouse].isLocked();
}
bool MouseObservable::isLocked() const {
    for (auto& lock : mLocks) {
        if (lock.isLocked()) {
            return true;
        }
    }

    return false;
}

const std::function<void(Event::MouseButton, bool)>&
MouseObservable::getOnClick(SubscriptionPtr sub, Event::Mouse btn) {
    switch (btn) {
        case Event::Mouse::LEFT:
            return sub->get<ON_LEFT>();
        case Event::Mouse::RIGHT:
            return sub->get<ON_RIGHT>();
        case Event::Mouse::MIDDLE:
            return sub->get<ON_MIDDLE>();
    }

    throw std::runtime_error("MouseObservable::getOnClick(): Invalid mouse: " +
                             std::to_string(btn));

    return DO_NOTHING;
}

void MouseObservable::onEvent(Event e) {
    for (auto mouse :
         {Event::Mouse::LEFT, Event::Mouse::RIGHT, Event::Mouse::MIDDLE}) {
        if (e[mouse].clicked()) {
            next(e[mouse]);
        }
    }
}
}  // namespace EventServices
