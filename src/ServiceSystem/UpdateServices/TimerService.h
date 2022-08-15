#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include <cmath>

#include "ServiceSystem/Component.h"
#include "ServiceSystem/CoreServices/UpdateService.h"
#include "ServiceSystem/Observable.h"
#include "ServiceSystem/Service.h"
#include "ServiceSystem/ServiceSystem.h"
#include "Utils/Time.h"

struct Timer {
    friend class TimerObservableBase;

    Timer() = default;
    Timer(int len);

    bool isActive() const;
    float getPercent() const;

    int length = 0;
    int timer = 0;

   private:
    bool active = true;
};

typedef Observable<bool(Timer&), void(Time, Timer&), Timer> TimerObservableType;

class TimerObservableBase : public TimerObservableType, public Component {
   public:
    typedef SubscriptionTypePtr<std::function<void(Time)>> UpdateSubPtr;

    enum : size_t { ON_TRIGGER = 0, ON_UPDATE, DATA };

    using TimerObservableType::subscribe;
    SubscriptionPtr subscribe(std::function<bool(Timer&)> func,
                              const Timer& timer);

    void onSubscribe(SubscriptionPtr sub);

   protected:
    virtual UpdateSubPtr getUpdateSub(std::function<void(Time)> func);

   private:
    void init();

    void onUpdate(Time dt);

    UpdateSubPtr mUpdateSub;
};

class TimerObservable : public TimerObservableBase {
   private:
    UpdateSubPtr getUpdateSub(std::function<void(Time)> func);
};

class TimerService : public Service<TimerObservable> {};

#endif
