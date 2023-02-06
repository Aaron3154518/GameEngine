#include "Messager.h"

#include <Framework/EventSystem/TimerService.h>

namespace Messages {
// Messager
Messager::Messager() : mId(Entities::generateUUID()) {}
Messager::~Messager() {
    for (auto& handle : mSubscriptions) {
        GetMessageBus().unsubscribe(handle);
    }
}

Messager::operator Entities::UUID() const { return mId; }

Entities::UUID Messager::id() const { return mId; }

void Messager::startTimer(int len_ms, const std::function<void()>& callback) {
    mSubscriptions.push_back(
        GetMessageBus().subscribe<TimerService::TimerMessage>(
            [callback](const auto& m) { callback(); },
            GameObjects::Get<TimerService>().startTimer(len_ms),
            TimerService::Finished));
}

void Messager::init() {}
}  // namespace Messages
