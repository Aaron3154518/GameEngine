#include "KeyPressObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
// KeyPressObservable
void KeyPressObservable::init() {
    mEventSub =
        GetEventObservable()->subscribe([this](const Event& e) { onEvent(e); });
}

void KeyPressObservable::next(const std::vector<SDL_Keycode>& keys) {}

void KeyPressObservable::onEvent(const Event& e) {}
}  // namespace EventServices