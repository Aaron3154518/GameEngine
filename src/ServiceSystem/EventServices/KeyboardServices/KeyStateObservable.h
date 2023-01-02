#ifndef KEY_STATE_SERVICE_H
#define KEY_STATE_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <memory>
#include <vector>

namespace EventServices {
typedef Observable<void(SDL_Keycode), void(SDL_Keycode),
                   void(SDL_Keycode, uint32_t), std::vector<SDL_Keycode>>
    KeyStateObservableBase;

class KeyStateObservable : public KeyStateObservableBase {};

typedef std::shared_ptr<KeyStateObservable> KeyStateObservablePtr;
}  // namespace EventServices

#endif
