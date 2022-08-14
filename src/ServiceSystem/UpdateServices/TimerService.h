#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include "ServiceSystem/Component.h"
#include "ServiceSystem/CoreServices/UpdateService.h"
#include "ServiceSystem/Observable.h"
#include "ServiceSystem/Service.h"
#include "ServiceSystem/ServiceSystem.h"
#include "Utils/Time.h"

struct Timer {
    friend class TimerObservable;

    Timer() = default;
    Timer(int len);

    bool isActive() const;

    int length = 0;
    int timer = 0;

   private:
    bool active = true;
};

typedef Observable<bool(), void(Time, Timer&), Timer> TimerObservableBase;

class TimerObservable : public TimerObservableBase, public Component {
   public:
    enum : size_t { ON_TRIGGER = 0, ON_UPDATE, DATA };

    using TimerObservableBase::subscribe;
    SubscriptionPtr subscribe(std::function<bool()> func, const Timer& timer);

    void onSubscribe(SubscriptionPtr sub);

   private:
    void init();

    void onUpdate(Time dt);

    UpdateObservable::SubscriptionPtr mUpdateSub;
};

class TimerService : public Service<TimerObservable> {};

#endif
