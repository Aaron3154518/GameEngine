#include "TimerService.h"

// TimerObservable
void TimerObservable::init() {
    mUpdateSub =
        ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
            std::bind(&TimerObservable::onUpdate, this, std::placeholders::_1));
}

void TimerObservable::onUpdate(Time dt) {
    for (auto it = begin(); it != end(); ++it) {
        Timer& data = (*it)->get<DATA>();
        data.timer += dt;
        while (data.timer >= data.length) {
            data.timer -= data.length;
            if (!(*it)->get<FUNC>()()) {
                it = erase(it);
                if (it == end()) {
                    return;
                }
                break;
            }
        }
    }
}