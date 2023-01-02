#ifndef HOVER_SERVICE_H
#define HOVER_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/EventService.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/MouseServices/MouseService.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <algorithm>
#include <memory>
#include <set>

typedef Observable<void(), void(SDL_Point), void(), UIComponentPtr>
    HoverObservableBase;

class HoverObservable : public Component, public HoverObservableBase {
   public:
    enum : size_t { ON_MOUSE_ENTER = 0, ON_HOVER, ON_MOUSE_LEAVE, DATA };

    void onSubscribe(SubscriptionPtr sub);

    void next(SDL_Point mouse);

   private:
    void init();

    void onEvent(Event e);

    SubscriptionWPtr mCurrHover;
    EventObservable::SubscriptionPtr mEventSub;
};

class HoverService : public Service<HoverObservable> {};

#endif
