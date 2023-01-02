#ifndef TYPING_SERVICE_H
#define TYPING_SERVICE_H

#include <ServiceSystem/Component.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <memory>

namespace EventServices {
typedef Observable<void(const std::string&)> TypingObservableBase;

class TypingObservable : public TypingObservableBase {};

typedef std::shared_ptr<TypingObservable> TypingObservablePtr;
}  // namespace EventServices

#endif