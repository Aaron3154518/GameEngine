#include "MouseObservable.h"

void MouseObservable::next(Event::MouseButton mouse)
{
    sort();

    bool foundTop = false;
    // std::cerr << "Mouse Next" << std::endl;
    forEachSubscription(
        [&](SubscriptionPtr sub) -> bool
        {
            if (!foundTop && SDL_PointInRect(&mouse.clickPos, &sub->data->rect))
            {
                (*sub)(mouse, true);
                foundTop = true;
            }
            else
            {
                (*sub)(mouse, false);
            }
            return true;
        });
}

void MouseObservable::sort()
{
    mSubscriptions.sort([](const SubscriptionPtr &a, const SubscriptionPtr &b) -> bool
                        { return a->data->elevation > b->data->elevation; });
}