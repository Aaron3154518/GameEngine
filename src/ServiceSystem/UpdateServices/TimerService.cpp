#include "TimerService.h"

// Timer
Timer::Timer(int len) : length(len), timer(len) {}

bool Timer::isActive() const { return active; }

// TimerObservable
TimerObservable::SubscriptionPtr TimerObservable::subscribe(
    std::function<bool()> func, const Timer& timer) {
    return subscribe(
        func, [](Time dt, Timer& t) {}, timer);
}

void TimerObservable::onSubscribe(SubscriptionPtr sub) {
    sub->get<DATA>().active = true;
}

void TimerObservable::init() {
    mUpdateSub =
        ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
            std::bind(&TimerObservable::onUpdate, this, std::placeholders::_1));
}

void TimerObservable::onUpdate(Time dt) {
    for (auto it = begin(); it != end(); ++it) {
        auto sub = *it;
        Timer& data = sub->get<DATA>();
        data.timer -= dt;
        sub->get<ON_UPDATE>()(dt, data);
        while (data.timer <= 0) {
            if (!sub->get<ON_TRIGGER>()()) {
                data.active = false;
                it = erase(it);
                if (it == end()) {
                    return;
                }
                break;
            }
            data.timer += data.length;
        }
    }
}
