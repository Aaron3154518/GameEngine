#include "RenderService.h"

#include "../Game.h"

// RenderOrderObservable
void RenderOrderObservable::sort() {
    std::stable_sort(mRenderOrder.begin(), mRenderOrder.end(),
                     [](const UIComponentPtr &a, const UIComponentPtr &b) {
                         return a->elevation < b->elevation;
                     });
    next(mRenderOrder);
}

void RenderOrderObservable::addComponent(UIComponentPtr comp) {
    int &count = mRefCounts[comp];
    if (count == 0) {
        mRenderOrder.push_back(comp);
    }
    count++;
}

void RenderOrderObservable::removeComponent(UIComponentPtr comp) {
    auto refIt = mRefCounts.find(comp);
    auto it = std::find(mRenderOrder.begin(), mRenderOrder.end(), comp);
    if (refIt == mRefCounts.end()) {
        if (it != mRenderOrder.end()) {
            mRenderOrder.erase(it);
        }
    } else if (it == mRenderOrder.end()) {
        mRefCounts.erase(refIt);
    } else {
        refIt->second--;
        if (refIt->second < 1) {
            mRenderOrder.erase(it);
            mRefCounts.erase(refIt);
        }
    }
}

// RenderObservable
RenderObservable::SubscriptionPtr RenderObservable::subscribe(Subscription::Function func, UIComponentPtr data) {
    SubscriptionPtr retVal =
        Observable<SDL_Renderer *, void(SDL_Renderer *), UIComponent>::subscribe(func, data);
    Game::gameStruct().mServices.renderService.addComponent(data);
    return retVal;
}

bool RenderObservable::unsubscribe(SubscriptionPtr sub) {
    Game::gameStruct().mServices.renderService.removeComponent(sub->data);
    return true;
}

void RenderObservable::sort(const std::vector<UIComponentPtr> &order) {
    std::unordered_map<SubscriptionPtr, int> idxs;

    // Map out the order position of each subscription
    for (auto sub : mSubscriptions) {
        idxs[sub] = std::find(order.begin(), order.end(), sub->data) - order.begin();
    }

    // Sort the subcription by ascending order position
    mSubscriptions.sort([&idxs](const SubscriptionPtr &a, const SubscriptionPtr &b) -> bool {
        return idxs.at(a) <= idxs.at(b);
    });
}

// RenderService
RenderService::RenderService() {
    Game::registerComponent(this);
}

void RenderService::init(GameStruct &gs) {
    gs.mServices.updateService.update$.subscribe(
        [this](Time dt) {
            renderOrder$.sort();
        });
    gs.mServices.renderService.renderOrder$.subscribe(
        [this](const std::vector<UIComponentPtr> &order) {
            render$.sort(order);
        });
}

void RenderService::addComponent(UIComponentPtr comp) {
    renderOrder$.addComponent(comp);
}

void RenderService::removeComponent(UIComponentPtr comp) {
    renderOrder$.removeComponent(comp);
}