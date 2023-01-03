#ifndef SCROLL_SERVICE_H
#define SCROLL_SERVICE_H

#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <memory>

namespace EventServices {
typedef Observable<void(float), UIComponentPtr> ScrollObservableBase;

class ScrollObservable : public Component, public ScrollObservableBase {
   public:
    enum : uint8_t { ON_SCROLL, POS };

   private:
    void init();

    void onEvent(const Event& e);

    void next(int scroll);

    EventObservable::SubscriptionPtr mEventSub;
};

typedef std::shared_ptr<ScrollObservable> ScrollObservablePtr;
}  // namespace EventServices

#endif
