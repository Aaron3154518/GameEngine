#include "DragObservable.h"

#include <ServiceSystem/EventServices/EventService.h>

namespace EventServices {
// DragObservable
void DragObservable::init() {
    updateSub =
        ServiceSystem::Get<UpdateService, UpdateObservable>()->subscribe(
            [this](Time dt) { onUpdate(dt); });
    eventSub =
        GetEventObservable()->subscribe([this](const Event &e) { onEvent(e); });
}

void DragObservable::onSubscribe(SubscriptionPtr sub) {
    ServiceSystem::Get<RenderService, RenderOrderObservable>()->addComponent(
        sub);
}

void DragObservable::next(const Event &e) {
    auto left = e[Event::LEFT];
    SDL_Point mousePos = e.mouse();

    if (current) {
        auto uiData = current->get<UI_DATA>();
        auto dragData = current->get<DRAG_DATA>();
        if (!dragData->dragging || !uiData->visible || left.released()) {
            current->get<DRAG_END>()();
            dragData->dragging = false;
            current.reset();
            // Release the lock on next update
        } else {
            current->get<DRAG>()(mousePos.x, mousePos.y, e.mouseDx(),
                                 e.mouseDy());
        }
    } else if (left.held()) {
        auto underMouse =
            ServiceSystem::Get<RenderService, RenderOrderObservable>()
                ->getUnderMouse();

        for (auto sub : *this) {
            auto uiData = sub->get<UI_DATA>();
            auto dragData = sub->get<DRAG_DATA>();

            if (!uiData->visible || !uiData->mouse) {
                continue;
            }

            if (uiData == underMouse) {
                int dragDelay = dragData->dragDelay;
                if ((dragDelay < 0 && e.mouseMoved()) ||
                    (dragDelay >= 0 && left.duration >= dragDelay)) {
                    current = sub;
                    mouseLock = GetMouseObservable()->requestLock();
                    dragData->dragging = true;
                    current->get<DRAG_START>()();
                    current->get<DRAG>()(mousePos.x, mousePos.y, e.mouseDx(),
                                         e.mouseDy());
                }
                break;
            }
        }
    }
}

void DragObservable::onUpdate(Time dt) {
    if (mouseLock && !current) {
        GetMouseObservable()->releaseLock(mouseLock);
    }
}

void DragObservable::onEvent(const Event &e) { next(e); }
}  // namespace EventServices