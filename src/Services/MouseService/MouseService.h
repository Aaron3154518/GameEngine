#ifndef MOUSE_SERVICE_H
#define MOUSE_SERVICE_H

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

class MouseService : private BaseObservable<Event::MouseButton>
{
public:
    Unsubscribe subscribe(Subscription sub, std::shared_ptr<UIComponent> comp);

    void next(Event::MouseButton mouse);

private:
    void sort();

    typedef std::pair<std::shared_ptr<UIComponent>, Subscription> Component;
    std::vector<Component> mComponents;
};

#endif
