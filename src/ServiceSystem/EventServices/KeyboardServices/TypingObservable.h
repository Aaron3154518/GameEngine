#ifndef TYPING_SERVICE_H
#define TYPING_SERVICE_H

#include <ServiceSystem/Component.h>
#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/Lockable.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <cmath>
#include <memory>
#include <sstream>

namespace EventServices {
// Full text, new text
typedef Observable<void(const std::string&, const std::string&), Lock>
    TypingObservableBase;

class TypingObservable : public Component, public TypingObservableBase {
    struct Composition {
        std::stringstream start, end;
    };

   public:
    enum : uint8_t { ON_INPUT = 0, LOCK };

    SubscriptionPtr subscribe(
        std::function<void(const std::string&, const std::string&)> onInput);

    void requestKeyboard(SubscriptionPtr sub);
    void releaseKeyboard(SubscriptionPtr sub);
    bool keyboardActive();

   private:
    void init();

    void onEvent(const Event& e);

    SubscriptionPtr getActiveSub();

    Lockable mLocks;
    std::unordered_map<Lock, Composition> mText;

    EventObservable::SubscriptionPtr mEventSub;
};

typedef std::shared_ptr<TypingObservable> TypingObservablePtr;
}  // namespace EventServices

#endif