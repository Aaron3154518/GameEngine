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
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

struct UIComponent {
    UIComponent() = default;
    UIComponent(Rect r, int e) : rect(r), elevation(e) {}

    Rect rect;
    int elevation = 0;
    bool visible = true, mouse = true;
};

typedef std::shared_ptr<UIComponent> UIComponentPtr;
typedef std::shared_ptr<const UIComponent> UIComponentCPtr;

struct UIComponentCompare {
    bool operator()(const UIComponentPtr &a, const UIComponentPtr &b) const;
};

class RenderOrderObservable
    : public Observable<void(const std::unordered_map<UIComponentPtr, int> &)> {
   public:
    void next();

    void computeUnderMouse(SDL_Point mouse);
    UIComponentPtr getUnderMouse() const;

    void addComponent(SubscriptionTypeWPtr<UIComponentPtr> sub);

    const std::unordered_map<UIComponentPtr, int> &getOrder() const;

   private:
    void sort();

    std::list<SubscriptionTypeWPtr<UIComponentPtr>> mToAdd, mComponents;
    UIComponentPtr mUnderMouse;
    std::unordered_map<UIComponentPtr, int> mRenderOrder;
};

typedef Observable<void(SDL_Renderer *), UIComponentPtr> RenderObservableBase;

class RenderObservable : public Component, public RenderObservableBase {
   public:
    enum : size_t { FUNC = 0, DATA };

    void next(SDL_Renderer *renderer);

   private:
    void init();

    void sort(const std::unordered_map<UIComponentPtr, int> &order);

    void onSubscribe(SubscriptionPtr sub);

    RenderOrderObservable::SubscriptionPtr renderOrderSub;
};

class RenderService : public Service<RenderObservable, RenderOrderObservable> {
};

#endif