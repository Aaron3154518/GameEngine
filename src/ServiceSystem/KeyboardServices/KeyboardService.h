#ifndef KEYBOARD_SERVICE_H
#define KEYBOARD_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <vector>

typedef Observable<void(SDL_Keycode), std::vector<SDL_Keycode>>
    KeyPressObservableBase;
typedef Observable<void(SDL_Keycode), void(SDL_Keycode),
                   void(SDL_Keycode, uint32_t), std::vector<SDL_Keycode>>
    KeyStateObservableBase;

class KeyPressObservable : public KeyPressObservableBase {};

class KeyStateObservable : public KeyStateObservableBase {};

class KeyboardService : public Service<KeyPressObservable, KeyStateObservable> {
};

#endif
