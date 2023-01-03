#ifndef KEY_PRESS_SERVICE_H
#define KEY_PRESS_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <memory>
#include <vector>

namespace EventServices {
typedef Observable<void(SDL_Keycode), std::vector<SDL_Keycode>>
    KeyPressObservableBase;

class KeyPressObservable : public Component, public KeyPressObservableBase {
   public:
   private:
    void init();

    void next(const std::vector<SDL_Keycode>& keys);

    void onEvent(const Event& e);

    EventObservable::SubscriptionPtr mEventSub;
};

typedef std::shared_ptr<KeyPressObservable> KeyPressObservablePtr;
}  // namespace EventServices

#endif
