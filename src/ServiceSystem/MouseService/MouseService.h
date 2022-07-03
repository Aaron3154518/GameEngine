#ifndef MOUSE_SERVICE_H
#define MOUSE_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/EventService.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <algorithm>
#include <memory>

class MouseObservable : public Component, public Observable<Event::MouseButton, void(Event::MouseButton, bool), UIComponent> {
    friend class MouseService;

   public:
    SubscriptionPtr subscribe(SubscriptionT::Function func, UIComponentPtr data);

   private:
    void init();

    void serve(Event::MouseButton mouse);

    bool unsubscribe(SubscriptionPtr sub);

    void onEvent(Event e);

    void onRenderOrder(const std::vector<UIComponentPtr> &order);

    EventObservable::SubscriptionPtr eventSub;
    RenderOrderObservable::SubscriptionPtr renderSub;
};

class MouseService : public Service<MouseObservable> {};

#endif
