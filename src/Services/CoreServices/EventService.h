#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include "../../EventSystem/Event.h"
#include "../../Utils/Observable/Observable.h"
#include "../ServiceHandler.h"

typedef Observable<const Event &, void(const Event &)> EventObservable;

class EventService : public Service {
   public:
    EventService() = default;
    ~EventService() = default;

    EventObservable event$;
};

REGISTER_SERVICE(EventService);

#endif