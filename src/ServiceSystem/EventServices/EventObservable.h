#ifndef EVENT_OBSERVABLE_H
#define EVENT_OBSERVABLE_H

#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <memory>

namespace EventServices {
class EventObservable : public ForwardObservable<void(const Event&)> {};

typedef std::shared_ptr<EventObservable> EventObservablePtr;
}  // namespace EventServices

#endif
