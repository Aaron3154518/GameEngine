#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include <SDL.h>

#include <algorithm>
#include <set>
#include <unordered_map>
#include <vector>

#include "../../Utils/Observable/Observable.h"
#include "../../Utils/Rect/Rect.h"
#include "../Component.h"
#include "../Game.h"
#include "../ServiceHandler.h"

struct UIComponent {
    UIComponent() = default;
    UIComponent(Rect r, int e) : rect(r), elevation(e) {}

    Rect rect = Rect();
    int elevation = 0;
    bool visible = true;
};

typedef std::shared_ptr<UIComponent> UIComponentPtr;

struct UIComponentCompare {
    bool operator()(const UIComponentPtr &a, const UIComponentPtr &b) const {
        return a->elevation <= b->elevation;
    }
};

class RenderOrderObservable : public Observable<const std::vector<UIComponentPtr> &, void(const std::vector<UIComponentPtr> &)> {
    friend class RenderService;

   private:
    void sort();
    void addComponent(UIComponentPtr comp);
    void removeComponent(UIComponentPtr comp);

    std::vector<UIComponentPtr> mRenderOrder;
    std::unordered_map<UIComponentPtr, int> mRefCounts;
};

class RenderObservable : public Observable<SDL_Renderer *, void(SDL_Renderer *), UIComponent> {
    friend class RenderService;

   public:
    SubscriptionPtr subscribe(SubscriptionT::Function func, UIComponentPtr data);

   private:
    void serve(SDL_Renderer *renderer);

    bool unsubscribe(SubscriptionPtr sub);

    void sort(const std::vector<UIComponentPtr> &order);
};

class RenderService : public Service, public Component {
   public:
    RenderService();
    ~RenderService() = default;

    RenderOrderObservable renderOrder$;
    RenderObservable render$;

    void addComponent(UIComponentPtr comp);
    void removeComponent(UIComponentPtr comp);

   private:
    void init(GameStruct &gs);
};

REGISTER_SERVICE(RenderService);

#endif