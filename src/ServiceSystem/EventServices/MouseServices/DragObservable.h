#ifndef DRAG_SERVICE_H
#define DRAG_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/CoreServices/UpdateService.h>
#include <ServiceSystem/EventServices/EventObservable.h>
#include <ServiceSystem/EventServices/MouseServices/MouseObservable.h>
#include <ServiceSystem/Lockable.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <set>

namespace EventServices {
struct DragComponent {
    DragComponent() = default;
    DragComponent(int _dragDelay) : dragDelay(_dragDelay) {}

    int dragDelay = 0;
    bool dragging = false;
};

typedef std::shared_ptr<DragComponent> DragComponentPtr;

typedef Observable<void(), void(int, int, float, float), void(), UIComponentPtr,
                   DragComponentPtr>
    DragObservableBase;

class DragObservable : public Component, public DragObservableBase {
   public:
    enum : size_t { DRAG_START = 0, DRAG, DRAG_END, UI_DATA, DRAG_DATA };

    void onSubscribe(SubscriptionPtr sub);

   private:
    void init();

    void onUpdate(Time dt);

    void onEvent(const Event& e);

    void next(const Event& e);

    MouseObservable::Lock mouseLock;
    SubscriptionPtr current;

    UpdateObservable::SubscriptionPtr updateSub;
    EventObservable::SubscriptionPtr eventSub;
};

typedef std::shared_ptr<DragObservable> DragObservablePtr;
}  // namespace EventServices

#endif
