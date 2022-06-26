#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include "../../EventSystem/Event.h"
#include "../../Utils/Observable/Observable.h"
#include "../ServiceHandler.h"

typedef Observable<const Event &, void(const Event &)> EventObservable;

class EventService : public Service<EventObservable> {};

#endif