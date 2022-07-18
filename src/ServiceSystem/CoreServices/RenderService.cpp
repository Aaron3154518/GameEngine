#include "RenderService.h"

// UIComponentCompare
bool UIComponentCompare::operator()(const UIComponentPtr &a,
                                    const UIComponentPtr &b) const {
    return a->elevation <= b->elevation;
}

// RenderOrderObservable
void RenderOrderObservable::computeUnderMouse(const Event &e) {
    mUnderMouse.reset();

    SDL_Point mouse = e.mouse();
    for (auto it = mRenderOrder.rbegin(); it != mRenderOrder.rend(); ++it) {
        if ((*it)->visible && SDL_PointInRect(&mouse, (*it)->rect)) {
            mUnderMouse = *it;
            break;
        }
    }
}

UIComponentPtr RenderOrderObservable::getUnderMouse() const {
    return mUnderMouse;
}

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

const std::vector<UIComponentPtr> &RenderOrderObservable::getOrder() const {
    return mRenderOrder;
}

// RenderObservable
void RenderObservable::init() {
    // RenderService
    renderOrderSub =
        ServiceSystem::Get<RenderService, RenderOrderObservable>()->subscribe(
            std::bind(&RenderObservable::onRenderOrder, this,
                      std::placeholders::_1));
    renderOrderSub->setUnsubscriber(unsub);
}

void RenderObservable::onRenderOrder(const std::vector<UIComponentPtr> &order) {
    sort(order);
}

RenderObservable::SubscriptionPtr RenderObservable::subscribe(
    Subscription::Function func, UIComponentPtr data) {
    SubscriptionPtr retVal = RenderObservableBase::subscribe(func, data);
    ServiceSystem::Get<RenderService>()->addComponent(data);
    return retVal;
}

void RenderObservable::updateSubscriptionData(SubscriptionPtr sub,
                                              UIComponentPtr data) {
    ServiceSystem::Get<RenderService>()->removeComponent(sub->getData());
    RenderObservableBase::updateSubscriptionData(sub, data);
    ServiceSystem::Get<RenderService>()->addComponent(data);
}

void RenderObservable::serve(SDL_Renderer *renderer) {
    for (auto sub : mSubscriptions) {
        if (sub->getData()->visible) {
            call(sub, renderer);
        }
    }
}

bool RenderObservable::unsubscribe(SubscriptionPtr sub) {
    ServiceSystem::Get<RenderService>()->removeComponent(sub->getData());
    return true;
}

void RenderObservable::sort(const std::vector<UIComponentPtr> &order) {
    std::unordered_map<SubscriptionPtr, int> idxs;

    // Map out the order position of each subscription
    for (auto sub : mSubscriptions) {
        idxs[sub] = std::find(order.begin(), order.end(), sub->getData()) -
                    order.begin();
    }

    // Sort the subcription by ascending order position
    mSubscriptions.sort(
        [&idxs](const SubscriptionPtr &a, const SubscriptionPtr &b) -> bool {
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