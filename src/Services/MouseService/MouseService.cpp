#include "MouseService.h"

#include "../CoreServices/EventService.h"
#include "../CoreServices/RenderService.h"

// MouseObservable
MouseObservable::MouseObservable() {
    Game::registerComponent(this);
}

void MouseObservable::init(GameStruct &gs) {
    eventSub = ServiceHandler::Get<EventService>()->event$.subscribe(
        std::bind(&MouseObservable::onEvent, this, std::placeholders::_1));
    eventSub->setUnsubscriber(unsub);
    renderSub = ServiceHandler::Get<RenderService>()->renderOrder$.subscribe(
        std::bind(&MouseObservable::onRenderOrder, this, std::placeholders::_1));
    renderSub->setUnsubscriber(unsub);
}

MouseObservable::SubscriptionPtr MouseObservable::subscribe(SubscriptionT::Function func, UIComponentPtr data) {
    SubscriptionPtr retVal =
        Observable<Event::MouseButton, void(Event::MouseButton, bool), UIComponent>::subscribe(func, data);
    ServiceHandler::Get<RenderService>()->addComponent(data);
    return retVal;
}

bool MouseObservable::unsubscribe(SubscriptionPtr sub) {
    ServiceHandler::Get<RenderService>()->removeComponent(sub->data);
    return true;
}

void MouseObservable::serve(Event::MouseButton mouse) {
    bool foundTop = false;
    for (SubscriptionPtr sub : mSubscriptions) {
        if (!sub->data->visible) {
            continue;
        }

        if (!foundTop && SDL_PointInRect(&mouse.clickPos, &sub->data->rect)) {
            (*sub)(mouse, true);
            foundTop = true;
        } else {
            (*sub)(mouse, false);
        }
    }
}

void MouseObservable::onEvent(Event e) {
    if (e[Event::LEFT].clicked()) {
        next(e[Event::LEFT]);
    }
}

void MouseObservable::onRenderOrder(const std::vector<UIComponentPtr> &order) {
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
