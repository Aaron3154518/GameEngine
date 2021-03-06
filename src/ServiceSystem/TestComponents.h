#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/EventService.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/CoreServices/UpdateService.h>
#include <ServiceSystem/EventServices/DragService.h>
#include <ServiceSystem/EventServices/MouseService.h>
#include <ServiceSystem/EventServices/ResizeService.h>
#include <ServiceSystem/ServiceSystem.h>
#include <ServiceSystem/UpdateServices/TimerService.h>
#include <Utils/Colors.h>
#include <Utils/Event.h>
#include <Utils/Rect.h>
#include <Utils/Time.h>

#include <random>

class TestBase : public Component {
   public:
    TestBase(Rect r, int e);

    virtual SDL_Color getColor() const;

   protected:
    virtual void init();

    void onRender(SDL_Renderer *renderer);

    std::shared_ptr<UIComponent> mPos;
    RenderObservable::SubscriptionPtr mRenderSub;
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
    bool unsub = true;

    std::vector<UpdateObservable::SubscriptionPtr> mUpdateSubs;
    MouseObservable::SubscriptionPtr mMouseSub;
};

class MouseLockTest : public TestBase {
   public:
    MouseLockTest(Rect r, int e);

    SDL_Color getColor() const;

   private:
    void init();

    void onUpdate(Time dt);

    void onClick(Event::MouseButton b, bool clicked);

    void *mMouseLock = NULL;
    UpdateObservable::SubscriptionPtr mUpdateSub;
    MouseObservable::SubscriptionPtr mMouseSub;
};

class DragTest : public TestBase {
   public:
    DragTest(Rect r, int e, int d);

    SDL_Color getColor() const;

   private:
    void init();

    void onDragStart();
    void onDrag(int x, int y, float dx, float dy);
    void onDragEnd();

    SDL_Color dragColor = WHITE;
    DragComponentPtr mDrag;
    DragObservable::SubscriptionPtr mDragSub;
};

class TimerTest : public TestBase {
   public:
    TimerTest(Rect r, int e);

    SDL_Color getColor() const;

   private:
    void init();

    bool onTimer();

    bool color = false;

    TimerObservable::SubscriptionPtr mTimerSub;
};

class ResizeTest : public TestBase {
   public:
    ResizeTest();

    SDL_Color getColor() const;

   private:
    void init();

    void onResize(ResizeData data);

    ResizeObservable::SubscriptionPtr mResizeSub;
};

std::shared_ptr<TestBase> randomTestComponent(int w, int h);
