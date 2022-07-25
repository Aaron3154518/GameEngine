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

typedef Observable<void(Event::MouseButton, bool), UIComponentPtr>
    MouseObservableBase;

class MouseObservable : public Component, public MouseObservableBase {
   public:
    enum : size_t { FUNC = 0, DATA };

    void onSubscribe(SubscriptionPtr sub);

    void next(Event::MouseButton mouse);

    void* requestLock();
    void releaseLock(void*& lock);

   private:
    void init();

    void onEvent(Event e);

    std::set<std::unique_ptr<bool>> mLocks;
    EventObservable::SubscriptionPtr eventSub;
};

class MouseService : public Service<MouseObservable> {};

#endif
