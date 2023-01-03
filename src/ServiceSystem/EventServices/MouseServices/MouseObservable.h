#ifndef MOUSE_SERVICE_H
#define MOUSE_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/Lockable.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <algorithm>
#include <memory>
#include <set>

namespace EventServices {
typedef std::vector<Event::Mouse> MouseVector;

// Left, right, middle
typedef Observable<void(Event::MouseButton, bool),
                   void(Event::MouseButton, bool),
                   void(Event::MouseButton, bool), UIComponentPtr>
    MouseObservableBase;

class MouseObservable : public Component, public MouseObservableBase {
    typedef std::function<void(Event::MouseButton, bool)> ClickFunc;

   public:
    class Lock {
        friend class MouseObservable;

       public:
        Lock(const MouseVector& mice = {});

        const MouseVector& getMice() const;

        bool isLocked() const;

       private:
        void setLock(::Lock l, Event::Mouse mouse);
        ::Lock& getLock(Event::Mouse mouse);

        MouseVector mMice;
        std::unordered_map<Event::Mouse, ::Lock> mLocks;
    };

    enum : size_t { ON_LEFT = 0, ON_RIGHT, ON_MIDDLE, DATA };

    using MouseObservableBase::subscribe;
    SubscriptionPtr subscribe(ClickFunc onClick, UIComponentPtr pos,
                              const std::vector<Event::Mouse>& btns = {});
    SubscriptionPtr subscribeLeftClick(ClickFunc onClick, UIComponentPtr pos);
    SubscriptionPtr subscribeRightClick(ClickFunc onClick, UIComponentPtr pos);
    SubscriptionPtr subscribeMiddleClick(ClickFunc onClick, UIComponentPtr pos);

    void onSubscribe(SubscriptionPtr sub);

    Lock requestLock(const MouseVector& mice);
    void releaseLock(Lock& lock);
    bool isLocked(Event::Mouse mouse) const;
    bool isLocked() const;

    static const ClickFunc& getOnClick(SubscriptionPtr sub, Event::Mouse btn);
    static const ClickFunc& DO_NOTHING();

   private:
    void init();

    void onEvent(Event e);

    void next(Event::MouseButton mouse);

    EventObservable::SubscriptionPtr eventSub;

    std::array<Lockable, NUM_MICE> mLocks;
};

typedef std::shared_ptr<MouseObservable> MouseObservablePtr;
}  // namespace EventServices

#endif
