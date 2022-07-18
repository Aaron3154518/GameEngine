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
#include <set>

typedef Observable<Event::MouseButton, void(Event::MouseButton, bool), UIComponent> MouseObservableBase;

class MouseObservable : public Component, public MouseObservableBase {
   public:
    SubscriptionPtr subscribe(Subscription::Function func, UIComponentPtr data);
    void updateSubscriptionData(SubscriptionPtr sub, UIComponentPtr data);

    void* requestLock();
    void releaseLock(void*& lock);

   private:
    void init();

    void serve(Event::MouseButton mouse);

    bool unsubscribe(SubscriptionPtr sub);

    void onEvent(Event e);

    void onRenderOrder(const std::vector<UIComponentPtr>& order);

    std::set<std::unique_ptr<bool>> mLocks;
    EventObservable::SubscriptionPtr eventSub;
};

class MouseService : public Service<MouseObservable> {};

#endif
