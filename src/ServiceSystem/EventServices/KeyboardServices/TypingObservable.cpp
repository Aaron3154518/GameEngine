#include "TypingObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
// TypingObservable
TypingObservable::SubscriptionPtr TypingObservable::subscribe(
    std::function<void(const std::string&, size_t, const std::string&)>
        onInput) {
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

    auto& comp = mText[sub->get<LOCK>()];

    // Backspace and delete
    int backCnt = std::min(e.textInputBackspace(), (int)comp.start.tellp());
    if (backCnt > 0) {
        int size = (int)comp.start.tellp() - backCnt;
        comp.start.str(comp.start.str().substr(0, size));
        comp.start.seekp(size);
    }
    int delCnt = std::min(e.textInputDelete(), (int)comp.end.tellp());
    if (delCnt > 0) {
        std::string end = comp.end.str();
        comp.end.str("");
        comp.end << end.substr(delCnt);
    }

    // New characters
    comp.start << e.textInput();

    // Cursor move
    int textMove = e.textInputMove();
    if (e.textInputSeekStart()) {
        textMove = -comp.start.tellp();
    } else if (e.textInputSeekEnd()) {
        textMove = comp.end.tellp();
    }
    textMove = textMove >= 0 ? std::min(textMove, (int)comp.end.tellp())
                             : -std::min(-textMove, (int)comp.start.tellp());
    if (textMove > 0) {
        std::string end = comp.end.str();
        comp.start << end.substr(0, textMove);
        comp.end.str("");
        comp.end << end.substr(textMove);
    } else if (textMove < 0) {
        std::string start = comp.start.str();
        comp.start.str("");
        comp.start << start.substr(0, start.size() + textMove);
        std::string end = comp.end.str();
        comp.end.str("");
        comp.end << start.substr(start.size() + textMove) << end;
    }

    sub->get<ON_INPUT>()(comp.start.str() + comp.end.str(), comp.start.tellp(),
                         e.textInput());
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
    mText[lock] = Composition();
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