#include "RenderService.h"

#include "UpdateService.h"

// RenderOrderObservable
void RenderOrderObservable::next() {
    // Add pending
    for (auto &comp : mToAdd) {
        int &count = mRefCounts[comp];
        if (count == 0) {
            mRenderOrder.push_back(comp);
        }
        count++;
    }
    mToAdd.clear();
    // Sort
    sort();
    // Serve
    RenderOrderObservableBase::next(mRenderOrder);
}

void RenderOrderObservable::sort() {
    std::stable_sort(mRenderOrder.begin(), mRenderOrder.end(),
                     [](const UIComponentPtr &a, const UIComponentPtr &b) {
                         return a->elevation < b->elevation;
                     });
}

void RenderOrderObservable::addComponent(UIComponentPtr comp) {
    int &count = mRefCounts[comp];
    if (count == 0) {
        // If it's new, wait to add until next render cycle
        mToAdd.push_back(comp);
    } else {
        // If it's not new, immediately increment references
        count++;
    }
}

void RenderOrderObservable::removeComponent(UIComponentPtr comp) {
    // Check pending first
    auto compIt = std::find(mToAdd.begin(), mToAdd.end(), comp);
    if (compIt != mToAdd.end()) {
        mToAdd.erase(compIt);
        return;
    }

    // Else update references
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
void RenderObservable::init() {
    // RenderService
    renderOrderSub = ServiceHandler::Get<RenderService>()->Get<RenderOrderObservable>()->subscribe(
        std::bind(&RenderObservable::onRenderOrder, this, std::placeholders::_1));
    renderOrderSub->setUnsubscriber(unsub);
}

void RenderObservable::onRenderOrder(const std::vector<UIComponentPtr> &order) {
    sort(order);
    // next(renderer);
}

RenderObservable::SubscriptionPtr RenderObservable::subscribe(SubscriptionT::Function func, UIComponentPtr data) {
    SubscriptionPtr retVal =
        Observable<SDL_Renderer *, void(SDL_Renderer *), UIComponent>::subscribe(func, data);
    ServiceHandler::Get<RenderService>()->addComponent(data);
    return retVal;
}

void RenderObservable::serve(SDL_Renderer *renderer) {
    for (auto sub : mSubscriptions) {
        if (sub->data->visible) {
            (*sub)(renderer);
        }
    }
}

bool RenderObservable::unsubscribe(SubscriptionPtr sub) {
    ServiceHandler::Get<RenderService>()->removeComponent(sub->data);
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
void RenderService::addComponent(UIComponentPtr comp) {
    Get<RenderOrderObservable>()->addComponent(comp);
}

void RenderService::removeComponent(UIComponentPtr comp) {
    Get<RenderOrderObservable>()->removeComponent(comp);
}