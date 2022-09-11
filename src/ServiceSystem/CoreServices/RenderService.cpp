#include "RenderService.h"

// UIComponentCompare
bool UIComponentCompare::operator()(const UIComponentPtr &a,
                                    const UIComponentPtr &b) const {
    return a->elevation <= b->elevation;
}

// RenderOrderObservable
void RenderOrderObservable::computeUnderMouse(SDL_Point mouse) {
    mUnderMouse.reset();

    for (auto it = mComponents.rbegin(); it != mComponents.rend(); ++it) {
        auto compPtr = it->lock();
        if (compPtr && compPtr->mVal->visible && compPtr->mVal->mouse &&
            SDL_PointInRect(&mouse, compPtr->mVal->rect)) {
            mUnderMouse = compPtr->mVal;
            break;
        }
    }
}

void RenderOrderObservable::next() {
    // Add pending
    mComponents.insert(mComponents.end(), mToAdd.begin(), mToAdd.end());
    mToAdd.clear();
    // Sort
    sort();
    // Serve
    for (auto sub : *this) {
        sub->get<0>()(mRenderOrder);
    }
}

void RenderOrderObservable::sort() {
    // Prune deleted components
    for (auto it = mComponents.begin(), end = mComponents.end(); it != end;) {
        bool erased = false;
        while (it != end && !it->lock()) {
            it = mComponents.erase(it);
            erased = true;
        }
        if (!erased) {
            ++it;
        }
    }

    // Sort remaining components
    mComponents.sort([](const SubscriptionTypeWPtr<UIComponentPtr> &a,
                        const SubscriptionTypeWPtr<UIComponentPtr> &b) {
        return a.lock()->mVal->elevation < b.lock()->mVal->elevation;
    });

    // Create render order
    mUnderMouse.reset();
    if (mComponents.size() > 0) {
        mUnderMouse = mComponents.front().lock()->mVal;
    }
    mRenderOrder.clear();
    int i = 0;
    for (auto comp : mComponents) {
        int &order = mRenderOrder[comp.lock()->mVal];
        if (order == 0) {
            order = i++;
        }
    }
}

void RenderOrderObservable::addComponent(
    SubscriptionTypeWPtr<UIComponentPtr> sub) {
    mToAdd.push_back(sub);
}

UIComponentPtr RenderOrderObservable::getUnderMouse() const {
    return mUnderMouse;
}

const std::unordered_map<UIComponentPtr, int> &RenderOrderObservable::getOrder()
    const {
    return mRenderOrder;
}

// RenderObservable
void RenderObservable::init() {
    // RenderService
    renderOrderSub =
        ServiceSystem::Get<RenderService, RenderOrderObservable>()->subscribe(
            std::bind(&RenderObservable::sort, this, std::placeholders::_1));
}

void RenderObservable::onSubscribe(SubscriptionPtr sub) {
    ServiceSystem::Get<RenderService, RenderOrderObservable>()->addComponent(
        sub);
}

void RenderObservable::next(SDL_Renderer *renderer) {
    for (auto sub : *this) {
        if (sub->get<DATA>()->visible) {
            sub->get<FUNC>()(renderer);
        }
    }
}

void RenderObservable::sort(
    const std::unordered_map<UIComponentPtr, int> &order) {
    // Remove unsubscribed
    prune();
    // Sort the subcription by ascending order position
    mSubscriptions.sort(
        [&order](const SubscriptionWPtr &a, const SubscriptionWPtr &b) -> bool {
            return order.at(a.lock()->get<DATA>()) <=
                   order.at(b.lock()->get<DATA>());
        });
}
