#include "KeyStateObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
// KeyStateObservable
const KeyStateObservable::KeyFunc& KeyStateObservable::DO_NOTHING() {
    static KeyFunc DO_NOTHING = [](Event::KeyButton k) {};
    return DO_NOTHING;
}

KeyStateObservable::SubscriptionPtr KeyStateObservable::subscribe(
    KeyFunc onPress, KeyFunc onRelease, KeyFunc onHeld,
    const std::unordered_set<SDL_KeyCode>& keys) {
    return KeyStateObservableBase::subscribe(onPress, onRelease, onHeld, keys);
}

KeyStateObservable::SubscriptionPtr KeyStateObservable::subscribe(
    KeyFunc onKeyEvent, Event::Button keyBtn,
    const std::unordered_set<SDL_KeyCode>& keys) {
    return subscribe(
        keyBtn == Event::Button::PRESSED ? onKeyEvent : DO_NOTHING(),
        keyBtn == Event::Button::RELEASED ? onKeyEvent : DO_NOTHING(),
        keyBtn == Event::Button::HELD ? onKeyEvent : DO_NOTHING(), keys);
}

void KeyStateObservable::init() {
    mEventSub =
        GetEventObservable()->subscribe([this](const Event& e) { onEvent(e); });
}

void KeyStateObservable::next(const std::vector<Event::KeyButton>& keys) {
    for (auto sub : *this) {
        auto& set = sub->get<KEYS>();
        for (auto& key : keys) {
            if (set.empty() || set.find(key.key) != set.end()) {
                if (key.pressed()) {
                    sub->get<ON_PRESS>()(key);
                }
                if (key.released()) {
                    sub->get<ON_RELEASE>()(key);
                }
                if (key.held()) {
                    sub->get<ON_HELD>()(key);
                }
            }
        }
    }
}

void KeyStateObservable::onEvent(const Event& e) {
    next(e.keys([](const Event::KeyButton& k) {
        return k.pressed() || k.released() || k.held();
    }));
}
}  // namespace EventServices