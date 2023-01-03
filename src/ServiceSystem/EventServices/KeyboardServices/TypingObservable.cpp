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

    auto& currLock = mLocks.locks().back();
    for (auto sub : *this) {
        auto& lock = sub->get<LOCK>();
        if (lock == currLock.get()) {
            std::string newText = e.textInput();
            auto& ss = mText[lock];
            ss << newText;
            sub->get<ON_INPUT>()(ss.str(), newText);
            break;
        }
    }
}

void TypingObservable::requestKeyboard(SubscriptionPtr sub) {
    if (!keyboardActive()) {
        SDL_StartTextInput();
    }
    auto& lock = sub->get<LOCK>();
    mText[lock] = std::stringstream();
    if (lock) {
        mLocks.releaseLock(lock);
    }
    lock = mLocks.requestLock();
}
void TypingObservable::releaseKeyboard(SubscriptionPtr sub) {
    auto& lock = sub->get<LOCK>();
    auto it = mText.find(lock);
    if (it != mText.end()) {
        mText.erase(it);
    }
    mLocks.releaseLock(lock);
    if (!keyboardActive()) {
        SDL_StopTextInput();
    }
}
bool TypingObservable::keyboardActive() const { return mLocks.isLocked(); }
}  // namespace EventServices