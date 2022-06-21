#include "MouseService.h"

#include "../CoreServices/EventService.h"
#include "../CoreServices/RenderService.h"

// MouseObservable
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
        if (!foundTop && SDL_PointInRect(&mouse.clickPos, &sub->data->rect)) {
            (*sub)(mouse, true);
            foundTop = true;
        } else {
            (*sub)(mouse, false);
        }
    }
}

void MouseObservable::sort(const std::vector<UIComponentPtr> &order) {
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

// MouseService
MouseService::MouseService() {
    Game::registerComponent(this);
}

void MouseService::init(GameStruct &gs) {
    ServiceHandler::Get<EventService>()->event$.subscribe(
        [this](const Event &e) {
            if (e[Event::LEFT].clicked()) {
                mouse$.next(e[Event::LEFT]);
            }
        });
    ServiceHandler::Get<RenderService>()->renderOrder$.subscribe(
        [this](const std::vector<UIComponentPtr> &order) {
            mouse$.sort(order);
        });
}