#ifndef DRAG_SERVICE_H
#define DRAG_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/EventService.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/CoreServices/UpdateService.h>
#include <ServiceSystem/MouseService/MouseService.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <set>

struct DragComponent : public UIComponent {
    DragComponent(Rect r, int e, int d);

    int dragDelay = 0;
    bool dragging = false;

    std::function<void()> onDragStart, onDragEnd;
    std::function<void(int, int, double, double)> onDrag;
};

typedef std::shared_ptr<DragComponent> DragComponentPtr;

class DragObservable : public Component,
                       public Observable<const Event&, void(), DragComponent> {
   public:
    SubscriptionPtr subscribe(SubscriptionT::Function func, DragComponentPtr data);

   private:
    void init();

    void serve(const Event& e);

    bool unsubscribe(SubscriptionPtr sub);

    void onUpdate(Time dt);

    void onEvent(const Event& e);

    void onRenderOrder(const std::vector<UIComponentPtr>& order);

    void* mouseLock;
    SubscriptionPtr current;
    UpdateObservable::SubscriptionPtr updateSub;
    EventObservable::SubscriptionPtr eventSub;
    RenderOrderObservable::SubscriptionPtr renderSub;
};

class DragService : public Service<DragObservable> {};

#endif
