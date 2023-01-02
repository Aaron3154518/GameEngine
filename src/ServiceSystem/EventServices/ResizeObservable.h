#ifndef RESIZE_SERVICE_H
#define RESIZE_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>

#include <memory>

namespace EventServices {
struct ResizeData {
    int oldW, oldH, newW, newH;
};

class ResizeObservable : public Component,
                         public ForwardObservable<void(ResizeData)> {
   private:
    void init();

    void onEvent(const Event& e);

    EventObservable::SubscriptionPtr mEventSub;
};

typedef std::shared_ptr<ResizeObservable> ResizeObservablePtr;
}  // namespace EventServices

#endif