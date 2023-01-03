#ifndef TYPING_SERVICE_H
#define TYPING_SERVICE_H

#include <ServiceSystem/Component.h>
#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/Lockable.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <memory>
#include <sstream>

namespace EventServices {
// Full text, new text
typedef Observable<void(const std::string&, const std::string&), Lock>
    TypingObservableBase;

class TypingObservable : public Component, public TypingObservableBase {
   public:
    enum : uint8_t { ON_INPUT = 0, LOCK };

    SubscriptionPtr subscribe(
        std::function<void(const std::string&, const std::string&)> onInput);

    void requestKeyboard(SubscriptionPtr sub);
    void releaseKeyboard(SubscriptionPtr sub);
    bool keyboardActive() const;

   private:
    void init();

    void onEvent(const Event& e);

    Lockable mLocks;
    std::unordered_map<Lock, std::stringstream> mText;

    EventObservable::SubscriptionPtr mEventSub;
};

typedef std::shared_ptr<TypingObservable> TypingObservablePtr;
}  // namespace EventServices

#endif