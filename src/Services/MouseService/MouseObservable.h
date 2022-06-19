#ifndef MOUSE_OBSERVABLE_H
#define MOUSE_OBSERVABLE_H

#include <algorithm>
#include <map>
#include <memory>

#include <SDL.h>

#include "../../EventSystem/Event.h"
#include "../../Utils/Observable/Observable.h"
#include "../../Utils/Rect/Rect.h"

struct UIComponent
{
    Rect rect = Rect();
    int elevation = 0;
};

class MouseObservable : private BaseObservable<Event::MouseButton>
{
public:
    typedef BaseObservable<Event::MouseButton>::Subscription Subscription;

    Unsubscribe subscribe(Subscription sub, std::shared_ptr<UIComponent> comp);

    void next(Event::MouseButton mouse);

private:
    void sort();

    typedef std::pair<std::shared_ptr<UIComponent>, Subscription> Component;
    std::vector<Component> mComponents;
};

#endif
