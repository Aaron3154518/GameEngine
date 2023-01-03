#ifndef HOVER_SERVICE_H
#define HOVER_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/EventServices/MouseServices/MouseObservable.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <algorithm>
#include <memory>
#include <set>

namespace EventServices {
typedef Observable<void(), void(SDL_Point), void(), UIComponentPtr>
    HoverObservableBase;

class HoverObservable : public Component, public HoverObservableBase {
   public:
    enum : size_t { ON_MOUSE_ENTER = 0, ON_HOVER, ON_MOUSE_LEAVE, DATA };

    void onSubscribe(SubscriptionPtr sub);

   private:
    void init();

    void onEvent(Event e);

    void next(SDL_Point mouse);

    SubscriptionWPtr mCurrHover;
    EventObservable::SubscriptionPtr mEventSub;
};

typedef std::shared_ptr<HoverObservable> HoverObservablePtr;
}  // namespace EventServices

#endif
