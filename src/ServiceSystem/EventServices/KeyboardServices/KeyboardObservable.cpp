#include "KeyboardObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
// KeyboardObservable
const KeyboardObservable::KeyFunc& KeyboardObservable::DO_NOTHING() {
    static KeyFunc DO_NOTHING = [](Event::KeyButton k) {};
    return DO_NOTHING;
}

KeyboardObservable::SubscriptionPtr KeyboardObservable::subscribe(
    KeyFunc onPress, KeyFunc onRelease, KeyFunc onHeld,
    const std::unordered_set<SDL_KeyCode>& keys) {
    return KeyboardObservableBase::subscribe(onPress, onRelease, onHeld, keys);
}

KeyboardObservable::SubscriptionPtr KeyboardObservable::subscribe(
    KeyFunc onKeyEvent, Event::Button keyBtn,
    const std::unordered_set<SDL_KeyCode>& keys) {
    return subscribe(
        keyBtn == Event::Button::PRESSED ? onKeyEvent : DO_NOTHING(),
        keyBtn == Event::Button::RELEASED ? onKeyEvent : DO_NOTHING(),
        keyBtn == Event::Button::HELD ? onKeyEvent : DO_NOTHING(), keys);
}

void KeyboardObservable::next(const std::vector<Event::KeyButton>& keys) {
    if (GetTypingObservable()->keyboardActive()) {
        return;
    }

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

void KeyboardObservable::init() {
    mEventSub =
        GetEventObservable()->subscribe([this](const Event& e) { onEvent(e); });
}

void KeyboardObservable::onEvent(const Event& e) {
    next(e.keys([](const Event::KeyButton& k) {
        return k.pressed() || k.released() || k.held();
    }));
}
}  // namespace EventServices