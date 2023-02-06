#include "TimerService.h"

// TimerService
void TimerService::init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) { onUpdate(m.data); },
        EventSystem::Update);
}

Entities::UUID TimerService::startTimer(int len_ms) {
    auto id = Entities::generateUUID();
    mTimers.push_back({id, len_ms});
    return id;
}

void TimerService::onUpdate(Time dt) {
    // Use indices in case of new timers during iteration
    size_t n = mTimers.size();
    for (size_t i = 0; i < n;) {
        auto& timer = mTimers.at(i);
        timer.timer_ms -= dt.ms();
        if (timer.timer_ms <= 0) {
            Entities::UUID id = timer.id;
            mTimers.erase(mTimers.begin() + i);
            Messages::GetMessageBus().sendMessage(TimerMessage(Finished, {id}));
            n--;
        } else {
            i++;
        }
    }
}
