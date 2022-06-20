#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include "../../EventSystem/Event.h"
#include "../../Utils/Observable/Observable.h"

class EventService {
   public:
    EventService() = default;
    ~EventService() = default;

    Observable<const Event &, void(const Event &)> event$;
};

#endif