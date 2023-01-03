#ifndef KEYBOARD_SERVICE_H
#define KEYBOARD_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <memory>
#include <unordered_set>
#include <vector>

namespace EventServices {
// press, release, held
typedef Observable<void(Event::KeyButton), void(Event::KeyButton),
                   void(Event::KeyButton), std::unordered_set<SDL_KeyCode>>
    KeyboardObservableBase;

class KeyboardObservable : public Component, public KeyboardObservableBase {
    typedef std::function<void(Event::KeyButton)> KeyFunc;

   public:
    enum : uint8_t { ON_PRESS = 0, ON_RELEASE, ON_HELD, KEYS };

    SubscriptionPtr subscribe(KeyFunc onPress, KeyFunc onRelease,
                              KeyFunc onHeld,
                              const std::unordered_set<SDL_KeyCode>& keys = {});
    SubscriptionPtr subscribe(KeyFunc onKeyEvent, Event::Button keyBtn,
                              const std::unordered_set<SDL_KeyCode>& keys = {});

    void next(const std::vector<Event::KeyButton>& keys);

    static const KeyFunc& DO_NOTHING();

   private:
    void init();

    void onEvent(const Event& e);

    EventObservable::SubscriptionPtr mEventSub;
};

typedef std::shared_ptr<KeyboardObservable> KeyboardObservablePtr;
}  // namespace EventServices

#endif
