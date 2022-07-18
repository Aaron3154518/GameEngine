#include "TimerService.h"

// TimerObservable
TimerObservable::SubscriptionPtr TimerObservable::subscribe(
    Subscription::Function func, int length) {
    return TimerObservableBase::subscribe(
        func, std::make_shared<Timer>(Timer{length, 0}));
}

void TimerObservable::init() {
    mUpdateSub =
        ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
            std::bind(&TimerObservable::onUpdate, this, std::placeholders::_1));
}

void TimerObservable::onUpdate(Time dt) {
    TimerObservableBase::removeUnsubscribed();

    for (auto it = mSubscriptions.begin(); it != mSubscriptions.end(); it++) {
        std::shared_ptr<Timer> data = (*it)->getData();
        data->timer += dt;
        while (data->timer >= data->length) {
            data->timer -= data->length;
            if (!(**it)()) {
                it = mSubscriptions.erase(it);
                if (it == mSubscriptions.end()) {
                    return;
                }
                break;
            }
        }
    }
}