#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include "../../Utils/Event.h"
#include "..//Observable.h"
#include "../ServiceSystem.h"

typedef Observable<const Event &, void(const Event &)> EventObservable;

class EventService : public Service<EventObservable> {};

#endif