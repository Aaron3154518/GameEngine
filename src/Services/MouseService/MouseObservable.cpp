#include "MouseObservable.h"

void MouseObservable::next(Event::MouseButton mouse)
{
    sort();

    bool foundTop = false;
    std::cerr << "Here" << std::endl;
    std::cerr << mSubscriptions.size() << std::endl;
    forEachSubscription(
        [&](Subscription &sub) -> bool
        {
            if (!foundTop && SDL_PointInRect(&mouse.clickPos, &sub.data->rect))
            {
                sub(mouse, true);
                foundTop = true;
            }
            else
            {
                sub(mouse, false);
            }
            return true;
        });
}

void MouseObservable::sort()
{
    mSubscriptions.sort([](const auto &a, const auto &b) -> bool
                        { return a.data->elevation > b.data->elevation; });
}