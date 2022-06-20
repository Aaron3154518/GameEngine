#ifndef MOUSE_SERVICE_H
#define MOUSE_SERVICE_H

#include <SDL.h>

#include <algorithm>
#include <map>
#include <memory>

#include "../../EventSystem/Event.h"
#include "../../Utils/Observable/Observable.h"
#include "../Component.h"
#include "../CoreServices/RenderService.h"

class MouseObservable : public Observable<Event::MouseButton, void(Event::MouseButton, bool), UIComponent> {
    friend class MouseService;

   public:
    SubscriptionPtr subscribe(Subscription::Function func, UIComponentPtr data);

   private:
    void serve(Event::MouseButton mouse);

    bool unsubscribe(SubscriptionPtr sub);

    void sort(const std::vector<UIComponentPtr> &order);
};

class MouseService : public Component {
   public:
    MouseService();

    MouseObservable mouse$;

   private:
    void init(GameStruct &gs);
};

#endif
