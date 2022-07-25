#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include "ServiceSystem/Component.h"
#include "ServiceSystem/CoreServices/UpdateService.h"
#include "ServiceSystem/Observable.h"
#include "ServiceSystem/Service.h"
#include "ServiceSystem/ServiceSystem.h"
#include "Utils/Time.h"

struct Timer {
    int length = 0;
    int timer = 0;
};

typedef Observable<bool(), Timer> TimerObservableBase;

class TimerObservable : public TimerObservableBase, public Component {
   public:
    enum : size_t { FUNC = 0, DATA };

   private:
    void init();

    void onUpdate(Time dt);

    UpdateObservable::SubscriptionPtr mUpdateSub;
};

class TimerService : public Service<TimerObservable> {};

#endif
