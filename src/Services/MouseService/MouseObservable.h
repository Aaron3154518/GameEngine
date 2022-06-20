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
    UIComponent(Rect r, int e) : rect(r), elevation(e) {}

    Rect rect = Rect();
    int elevation = 0;
};

class MouseObservable : public Observable<Event::MouseButton, void(Event::MouseButton, bool), UIComponent>
{
public:
    void next(Event::MouseButton mouse);

private:
    void sort();
};

#endif
