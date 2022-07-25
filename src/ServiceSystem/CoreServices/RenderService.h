#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include <SDL.h>
#include <ServiceSystem/Observable.h>
#include <ServiceSystem/Service.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>
#include <Utils/Rect.h>

#include <algorithm>
#include <list>
#include <set>
#include <unordered_map>
#include <vector>

struct UIComponent {
    UIComponent() = default;
    UIComponent(Rect r, int e) : rect(r), elevation(e) {}

    Rect rect = Rect();
    int elevation = 0;
    bool visible = true;
};

typedef std::shared_ptr<UIComponent> UIComponentPtr;

typedef std::weak_ptr<SubDataBase<UIComponentPtr>> UIComponentSubWPtr;

struct UIComponentCompare {
    bool operator()(const UIComponentPtr &a, const UIComponentPtr &b) const;
};

class RenderOrderObservable
    : public Observable<void(const std::vector<UIComponentPtr> &)> {
   public:
    void next();

    void computeUnderMouse(const Event &e);
    UIComponentPtr getUnderMouse() const;

    void addComponent(UIComponentSubWPtr sub);
    void removeComponent(UIComponentSubWPtr sub);

    const std::vector<UIComponentPtr> &getOrder() const;

   private:
    void sort();

    UIComponentPtr mUnderMouse;
    std::list<UIComponentSubWPtr> mToAdd;
    std::vector<UIComponentSubWPtr> mRenderOrder;
    std::unordered_map<UIComponentSubWPtr, int> mRefCounts;
};

typedef Observable<void(SDL_Renderer *), UIComponentPtr> RenderObservableBase;

class RenderObservable : public Component, public RenderObservableBase {
   public:
    SubscriptionPtr subscribe(std::function<void(SDL_Renderer *)> func,
                              UIComponentPtr data);

   private:
    void init();

    void serve(SDL_Renderer *renderer);

    bool unsubscribe(SubscriptionPtr sub);

    void sort(const std::vector<UIComponentPtr> &order);

    void onRenderOrder(const std::vector<UIComponentPtr> &order);

    RenderOrderObservable::SubscriptionPtr renderOrderSub;
};

class RenderService : public Service<RenderObservable, RenderOrderObservable> {
   public:
    void addComponent(UIComponentPtr comp);
    void removeComponent(UIComponentPtr comp);
};

#endif