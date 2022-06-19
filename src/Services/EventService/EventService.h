#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include "../../Utils/Observable/Observable.h"
#include "../../EventSystem/Event.h"

class EventService
{
public:
    EventService() = default;
    ~EventService() = default;

    Observable<Event> event$;
};

#endif