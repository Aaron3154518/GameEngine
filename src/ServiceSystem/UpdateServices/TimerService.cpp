#include "TimerService.h"

// Timer
Timer::Timer(int len) : length(len), timer(len) {}

Timer::Timer(const AnimationData& animData) : Timer(animData.frame_ms) {}

void Timer::reset() { timer = length; }

bool Timer::isActive() const { return active; }

int Timer::getTimePassed() const { return length - timer; }

int Timer::getTimeLeft() const { return timer; }

float Timer::getPercent() const { return fmax(0, getTimePassed()) / length; }

// TimerObservableBase
TimerObservableBase::SubscriptionPtr TimerObservableBase::subscribe(
    std::function<bool(Timer&)> func, const Timer& timer) {
    return subscribe(
        func, [](Time dt, Timer& t) {}, timer);
}

void TimerObservableBase::onSubscribe(SubscriptionPtr sub) {
    sub->get<DATA>().active = true;
}

TimerObservableBase::UpdateSubPtr TimerObservableBase::getUpdateSub(
    std::function<void(Time)> func) {
    return nullptr;
}

void TimerObservableBase::init() {
    mUpdateSub = getUpdateSub(
        std::bind(&TimerObservableBase::onUpdate, this, std::placeholders::_1));
}

void TimerObservableBase::onUpdate(Time dt) {
    for (auto it = begin(); it != end();) {
        auto sub = *it;
        Timer& data = sub->get<DATA>();
        data.timer -= dt;
        sub->get<ON_UPDATE>()(dt, data);
        bool erased = false;
        while (data.timer <= 0) {
            if (!sub->get<ON_TRIGGER>()(data)) {
                data.active = false;
                it = erase(it);
                erased = true;
                break;
            }
            data.timer += data.length;
        }
        if (!erased) {
            ++it;
        }
    }
}

// TimerObservable
TimerObservable::UpdateSubPtr TimerObservable::getUpdateSub(
    std::function<void(Time)> func) {
    return ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
        func);
}
