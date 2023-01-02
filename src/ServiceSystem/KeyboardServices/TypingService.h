#ifndef TYPING_SERVICE_H
#define TYPING_SERVICE_H

#include <ServiceSystem/Component.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

typedef Observable<void(const std::string&)> TypingObservableBase;

class TypingObservable : public TypingObservableBase {};

class KeyboardService : public Service<TypingObservable> {};

#endif