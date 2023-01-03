#include "MouseObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
const MouseObservable::ClickFunc& MouseObservable::DO_NOTHING() {
    const static ClickFunc DO_NOTHING = [](Event::MouseButton b, bool c) {};
    return DO_NOTHING;
}

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
    ClickFunc onClick, UIComponentPtr pos,
    const std::vector<Event::Mouse>& btns) {
    if (btns.empty()) {
        return subscribe(onClick, onClick, onClick, pos);
    }

    auto bIt = btns.begin(), eIt = btns.end();

    bool l = std::find(bIt, eIt, Event::Mouse::LEFT) != eIt;
    bool r = std::find(bIt, eIt, Event::Mouse::RIGHT) != eIt;
    bool m = std::find(bIt, eIt, Event::Mouse::MIDDLE) != eIt;

    return subscribe(l ? onClick : DO_NOTHING(), r ? onClick : DO_NOTHING(),
                     m ? onClick : DO_NOTHING(), pos);
}

MouseObservable::SubscriptionPtr MouseObservable::subscribeLeftClick(
    ClickFunc onClick, UIComponentPtr pos) {
    return subscribe(onClick, pos, {Event::Mouse::LEFT});
}
MouseObservable::SubscriptionPtr MouseObservable::subscribeRightClick(
    ClickFunc onClick, UIComponentPtr pos) {
    return subscribe(onClick, pos, {Event::Mouse::RIGHT});
}
MouseObservable::SubscriptionPtr MouseObservable::subscribeMiddleClick(
    ClickFunc onClick, UIComponentPtr pos) {
    return subscribe(onClick, pos, {Event::Mouse::MIDDLE});
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

const MouseObservable::ClickFunc& MouseObservable::getOnClick(
    SubscriptionPtr sub, Event::Mouse btn) {
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

    return DO_NOTHING();
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
