#ifndef DRAG_SERVICE_H
#define DRAG_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/EventService.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/CoreServices/UpdateService.h>
#include <ServiceSystem/EventServices/MouseService.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <set>

struct DragComponent {
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

    void next(const Event& e);

   private:
    void init();

    void onUpdate(Time dt);

    void onEvent(const Event& e);

    void* mouseLock;
    SubscriptionPtr current;

    UpdateObservable::SubscriptionPtr updateSub;
    EventObservable::SubscriptionPtr eventSub;
};

class DragService : public Service<DragObservable> {};

#endif
