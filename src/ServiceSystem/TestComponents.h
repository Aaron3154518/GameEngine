#include "ServiceSystem/Component.h"
#include "ServiceSystem/CoreServices/EventService.h"
#include "ServiceSystem/CoreServices/RenderService.h"
#include "ServiceSystem/CoreServices/UpdateService.h"
#include "ServiceSystem/MouseService/MouseService.h"
#include "ServiceSystem/ServiceSystem.h"
#include "Utils/Colors.h"
#include "Utils/Event.h"
#include "Utils/Rect.h"
#include "Utils/Time.h"

class TestBase : public Component {
   public:
    TestBase(Rect r, int e);

    const Rect &getRect() const;

    int getElevation() const;

    virtual SDL_Color getColor() const;

   protected:
    virtual void init();

    void onRender(SDL_Renderer *renderer);

    std::shared_ptr<UIComponent> mPos;
};

class ClickRenderTest : public TestBase {
   public:
    ClickRenderTest(Rect r, int e);

    SDL_Color getColor() const;

   private:
    void init();

    void onClick(Event::MouseButton b, bool clicked);

    bool color = false;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class ChangeSubTest : public TestBase {
   public:
    ChangeSubTest(Rect r, int e);

    SDL_Color getColor() const;

   private:
    void init();

    void onClick(Event::MouseButton b, bool clicked);

    bool color = false;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class UnsubTest : public TestBase {
   public:
    UnsubTest(Rect r, int e);

    SDL_Color getColor() const;

   private:
    void init();

    void onClick(Event::MouseButton b, bool clicked);

    bool color = false;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class UpdateTest : public TestBase {
   public:
    UpdateTest(Rect r, int e);

    SDL_Color getColor() const;

   private:
    void init();

    void onClick(Event::MouseButton b, bool clicked);

    void onUpdate(Time dt);

    int delayMs = 0;
    UpdateObservable::SubscriptionPtr mUpdateSub;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class VisibilityTest : public TestBase {
   public:
    VisibilityTest(Rect r, int e);

   private:
    void init();

    void onClick(Event::MouseButton b, bool clicked);

    void onUpdate(Time dt);

    int delayMs = 0;
    UpdateObservable::SubscriptionPtr mUpdateSub;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class InheritanceTestBase : public TestBase {
   public:
    InheritanceTestBase(Rect r, int e);

    SDL_Color getColor() const;

   protected:
    virtual void init();

    void onClick(Event::MouseButton b, bool clicked, bool red);

    SDL_Color color = BLACK;
    bool increaseColor = true;
    static const Uint8 COLOR_INC;

    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class InheritanceTestDerived : public InheritanceTestBase {
   public:
    InheritanceTestDerived(Rect r, int e);

   private:
    void init();
};

class MultiUnsubTest : public TestBase {
   public:
    MultiUnsubTest(Rect r, int e);

    SDL_Color getColor() const;

   private:
    void init();

    void onUpdate(Time dt);

    void onClick(Event::MouseButton b, bool clicked);

    void onRender(SDL_Renderer *renderer);

    int ctr = 0;
    Unsubscriber updateUnsub;
    std::vector<UpdateObservable::SubscriptionPtr> mUpdateSubs;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

std::shared_ptr<TestBase> randomTestComponent(int w, int h);
