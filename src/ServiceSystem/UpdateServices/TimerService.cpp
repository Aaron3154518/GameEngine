#include "TimerService.h"

// Timer
Timer::Timer(int len) : length(len), timer(len) {}

bool Timer::isActive() const { return active; }

float Timer::getPercent() const { return fmax(0, length - timer) / length; }

// TimerObservableBase
TimerObservableBase::SubscriptionPtr TimerObservableBase::subscribe(
    std::function<bool()> func, const Timer& timer) {
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

// TimerObservable
TimerObservable::UpdateSubPtr TimerObservable::getUpdateSub(
    std::function<void(Time)> func) {
    return ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
        func);
}
