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

struct UIComponentCompare {
    bool operator()(const UIComponentPtr &a, const UIComponentPtr &b) const;
};

// Use a typedef for less code duplication
typedef Observable<const std::vector<UIComponentPtr> &, void(const std::vector<UIComponentPtr> &)>
    RenderOrderObservableBase;

class RenderOrderObservable : public RenderOrderObservableBase {
   public:
    // Expose custom next
    void next();

    void computeUnderMouse(const Event &e);
    UIComponentPtr getUnderMouse() const;

    void addComponent(UIComponentPtr comp);
    void removeComponent(UIComponentPtr comp);

    const std::vector<UIComponentPtr> &getOrder() const;

   private:
    void sort();

    UIComponentPtr mUnderMouse;
    std::list<UIComponentPtr> mToAdd;
    std::vector<UIComponentPtr> mRenderOrder;
    std::unordered_map<UIComponentPtr, int> mRefCounts;
};

typedef Observable<SDL_Renderer *, void(SDL_Renderer *), UIComponent> RenderObservableBase;

class RenderObservable : public Component, public RenderObservableBase {
   public:
    SubscriptionPtr subscribe(Subscription::Function func, UIComponentPtr data);
    void updateSubscriptionData(SubscriptionPtr sub, UIComponentPtr data);

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