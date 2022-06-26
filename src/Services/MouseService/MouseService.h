#ifndef MOUSE_SERVICE_H
#define MOUSE_SERVICE_H

#include <SDL.h>

#include <algorithm>
#include <map>
#include <memory>

#include "../../Utils/Event.h"
#include "../Component.h"
#include "../CoreServices/EventService.h"
#include "../CoreServices/RenderService.h"
#include "../Observable.h"
#include "../ServiceHandler.h"

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
