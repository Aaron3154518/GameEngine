#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include <SDL.h>

#include <algorithm>
#include <list>
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

// Use a typedef for less code duplication
typedef Observable<const std::vector<UIComponentPtr> &, void(const std::vector<UIComponentPtr> &)>
    RenderOrderObservableBase;

class RenderOrderObservable : public RenderOrderObservableBase {
   public:
    // Expose custom next
    void next();

    void addComponent(UIComponentPtr comp);
    void removeComponent(UIComponentPtr comp);

   private:
    void sort();

    std::list<UIComponentPtr> mToAdd;
    std::vector<UIComponentPtr> mRenderOrder;
    std::unordered_map<UIComponentPtr, int> mRefCounts;
};

class RenderObservable : public Component, public Observable<SDL_Renderer *, void(SDL_Renderer *), UIComponent> {
    friend class RenderService;

   public:
    RenderObservable();

    SubscriptionPtr subscribe(SubscriptionT::Function func, UIComponentPtr data);

   private:
    void init(GameStruct &gs);

    void serve(SDL_Renderer *renderer);

    bool unsubscribe(SubscriptionPtr sub);

    void sort(const std::vector<UIComponentPtr> &order);

    void onRenderOrder(const std::vector<UIComponentPtr> &order);

    RenderOrderObservable::SubscriptionPtr renderOrderSub;
};

class RenderService : public Service {
   public:
    RenderService() = default;
    ~RenderService() = default;

    RenderOrderObservable renderOrder$;
    RenderObservable render$;

    void addComponent(UIComponentPtr comp);
    void removeComponent(UIComponentPtr comp);
};

REGISTER_SERVICE(RenderService);

#endif