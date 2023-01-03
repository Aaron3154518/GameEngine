#include "TypingObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
// TypingObservable
TypingObservable::SubscriptionPtr TypingObservable::subscribe(
    std::function<void(const std::string&, const std::string&)> onInput) {
    return TypingObservableBase::subscribe(onInput, Lock());
}

void TypingObservable::init() {
    mEventSub =
        GetEventObservable()->subscribe([this](const Event& e) { onEvent(e); });
}

void TypingObservable::onEvent(const Event& e) {
    if (!e.textInputUpdated()) {
        return;
    }

    auto sub = getActiveSub();

    if (!sub) {
        return;
    }

    auto& ss = mText[sub->get<LOCK>()];
    if (e.textInputBackspaced()) {
        int size = std::max(0, (int)ss.tellp() - 1);
        ss.str(ss.str().substr(0, size));
        ss.seekp(size);
    }
    ss << e.textInput();
    sub->get<ON_INPUT>()(ss.str(), e.textInput());
}

TypingObservable::SubscriptionPtr TypingObservable::getActiveSub() {
    auto lockIt = mLocks.locks().end();
    while (lockIt != mLocks.locks().begin()) {
        bool inactive = false;
        auto& currLock = *(--lockIt);
        for (auto it = abegin(); it != aend(); ++it) {
            if (it) {
                auto sub = *it;
                if (sub->get<LOCK>() != currLock.get()) {
                    continue;
                }
                if (sub->isActive()) {
                    return sub;
                }
                inactive = true;
                break;
            }
        }
        if (!inactive) {
            lockIt++;
            mLocks.releaseLock(currLock.get());
        }
    }

    return nullptr;
}

void TypingObservable::requestKeyboard(SubscriptionPtr sub) {
    auto& lock = sub->get<LOCK>();
    if (lock) {
        mLocks.releaseLock(lock);
    }
    mText[lock] = std::stringstream();
    lock = mLocks.requestLock();
}
void TypingObservable::releaseKeyboard(SubscriptionPtr sub) {
    auto& lock = sub->get<LOCK>();
    auto it = mText.find(lock);
    if (it != mText.end()) {
        mText.erase(it);
    }
    mLocks.releaseLock(lock);
}
bool TypingObservable::keyboardActive() { return (bool)getActiveSub(); }
}  // namespace EventServices