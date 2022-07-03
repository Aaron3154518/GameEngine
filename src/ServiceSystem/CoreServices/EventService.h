#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include "ServiceSystem/Observable.h"
#include "ServiceSystem/Service.h"
#include "Utils/Event.h"

typedef Observable<const Event &, void(const Event &)> EventObservable;

class EventService : public Service<EventObservable> {};

#endif