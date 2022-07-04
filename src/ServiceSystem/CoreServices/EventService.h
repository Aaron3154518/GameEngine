#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

typedef Observable<const Event &, void(const Event &)> EventObservableBase;

class EventObservable : public EventObservableBase {
   public:
    void next(const Event &e);
};

class EventService : public Service<EventObservable> {};

#endif