#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

typedef ForwardObservable<void(const Event &)> EventObservable;

class EventService : public Service<EventObservable> {};

#endif