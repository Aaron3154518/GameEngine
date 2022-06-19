#include "MouseObservable.h"

Unsubscribe MouseObservable::subscribe(MouseObservable::Subscription sub, std::shared_ptr<UIComponent> comp)
{
    mComponents.push_back(std::make_pair(comp, sub));
    return Unsubscribe{[this, comp]()
                       {
                           std::remove_if(mComponents.begin(), mComponents.end(), [comp](const Component &c) -> bool
                                          { return c.first == comp; });
                       }};
}

void MouseObservable::next(Event::MouseButton mouse)
{
    sort();
    for (auto &pair : mComponents)
    {
        if (SDL_PointInRect(&mouse.clickPos, &pair.first->rect))
        {
            pair.second(mouse);
        }
    }
}

void MouseObservable::sort()
{
    std::sort(mComponents.begin(), mComponents.end(),
              [](const Component &a, const Component &b) -> bool
              { return a.first->elevation > b.first->elevation; });
}