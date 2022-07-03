#include "DragService.h"

// DragComponent
DragComponent::DragComponent(Rect r, int e, int d) : UIComponent(r, e), dragDelay(d) {}

// DragObservable
void DragObservable::init() {
    updateSub = ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
        std::bind(&DragObservable::onUpdate, this, std::placeholders::_1));
    eventSub = ServiceSystem::Get<EventService>()->Get<EventObservable>()->subscribe(
        std::bind(&DragObservable::onEvent, this, std::placeholders::_1));
    eventSub->setUnsubscriber(unsub);
    renderSub = ServiceSystem::Get<RenderService>()->Get<RenderOrderObservable>()->subscribe(
        std::bind(&DragObservable::onRenderOrder, this, std::placeholders::_1));
    renderSub->setUnsubscriber(unsub);
}

DragObservable::SubscriptionPtr DragObservable::subscribe(SubscriptionT::Function func, DragComponentPtr data) {
    SubscriptionPtr retVal =
        Observable<const Event &, void(), DragComponent>::subscribe(func, data);
    ServiceSystem::Get<RenderService>()->addComponent(data);
    return retVal;
}

bool DragObservable::unsubscribe(SubscriptionPtr sub) {
    ServiceSystem::Get<RenderService>()->removeComponent(sub->data);
    return true;
}

void DragObservable::serve(const Event &e) {
    auto left = e[Event::LEFT];
    SDL_Point mousePos = e.mouse();

    if (current) {
        if (!current->data->dragging || !current->data->visible || left.released()) {
            current->data->onDragEnd();
            current->data->dragging = false;
            current.reset();
            // Release the lock on next update
        } else {
            current->data->onDrag(mousePos.x, mousePos.y, e.mouseDx(), e.mouseDy());
        }
    } else if (left.held()) {
        for (SubscriptionPtr sub : mSubscriptions) {
            if (!sub->data->visible) {
                continue;
            }

            if (SDL_PointInRect(&mousePos, &sub->data->rect)) {
                int dragDelay = sub->data->dragDelay;
                if ((dragDelay < 0 && e.mouseMoved()) || (dragDelay >= 0 && left.duration >= dragDelay)) {
                    current = sub;
                    mouseLock = ServiceSystem::Get<MouseService, MouseObservable>()->requestLock();
                    current->data->dragging = true;
                    current->data->onDragStart();
                    current->data->onDrag(mousePos.x, mousePos.y, e.mouseDx(), e.mouseDy());
                }
                break;
            }
        }
    }
}

void DragObservable::onUpdate(Time dt) {
    if (mouseLock && !current) {
        ServiceSystem::Get<MouseService, MouseObservable>()->releaseLock(mouseLock);
    }
}

void DragObservable::onEvent(const Event &e) {
    next(e);
}

void DragObservable::onRenderOrder(const std::vector<UIComponentPtr> &order) {
    std::unordered_map<SubscriptionPtr, int> idxs;

    // Map out the order position of each subscription
    for (auto sub : mSubscriptions) {
        idxs[sub] = std::find(order.begin(), order.end(), sub->data) - order.begin();
    }

    // Sort the subcription by ascending order position
    mSubscriptions.sort([&idxs](const SubscriptionPtr &a, const SubscriptionPtr &b) -> bool {
        return idxs.at(a) > idxs.at(b);
    });
}