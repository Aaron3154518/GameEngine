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
        if (compPtr && compPtr->mData->visible &&
            SDL_PointInRect(&mouse, compPtr->mData->rect)) {
            mUnderMouse = compPtr->mData;
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
        call<0>(*sub, getOrder());
    }
}

void RenderOrderObservable::sort() {
    // Prune deleted components
    for (auto it = mComponents.begin(), end = mComponents.end(); it != end;
         ++it) {
        while (it != end && !it->lock()) {
            it = mComponents.erase(it);
        }
        if (it == end) {
            break;
        }
    }

    // Sort remaining components
    std::stable_sort(
        mComponents.begin(), mComponents.end(),
        [](const UIComponentSubWPtr &a, const UIComponentSubWPtr &b) {
            return a.lock()->mData->elevation < b.lock()->mData->elevation;
        });

    // Create render order
    mUnderMouse = mComponents.front().lock()->mData;
    mRenderOrder.clear();
    int i = 0;
    for (auto comp : mComponents) {
        int &order = mRenderOrder[comp.lock()->mData];
        if (order == 0) {
            order = i++;
        }
    }
}

void RenderOrderObservable::addComponent(UIComponentSubWPtr sub) {
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

void RenderObservable::next(SDL_Renderer *renderer) {
    for (auto sub : *this) {
        if (get<0>(*sub)->visible) {
            call<0>(*sub, renderer);
        }
    }
}

void RenderObservable::sort(
    const std::unordered_map<UIComponentPtr, int> &order) {
    // Sort the subcription by ascending order position
    mSubscriptions.sort(
        [&order](const SubscriptionPtr &a, const SubscriptionPtr &b) -> bool {
            return order.at(get<0>(*a)) <= order.at(get<0>(*b));
        });
}

void RenderObservable::onSubscribe(SubscriptionPtr sub) {
    std::cerr << "RenderService onSubscribe" << std::endl;
    ServiceSystem::Get<RenderService, RenderOrderObservable>()->addComponent(
        sub);
}