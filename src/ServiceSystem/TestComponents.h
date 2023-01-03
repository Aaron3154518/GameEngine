#include <ServiceSystem/Component.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/CoreServices/UpdateService.h>
#include <ServiceSystem/EventServices/EventService.h>
#include <ServiceSystem/Lockable.h>
#include <ServiceSystem/ServiceSystem.h>
#include <ServiceSystem/UpdateServices/TimerService.h>
#include <Utils/Colors.h>
#include <Utils/Event.h>
#include <Utils/Rect.h>
#include <Utils/Time.h>

#include <algorithm>
#include <random>
#include <string>

using namespace EventServices;

class TestBase : public Component {
   public:
    TestBase(Rect r, int e);

    virtual SDL_Color getColor() const;

   protected:
    virtual void init();

    virtual void onRender(SDL_Renderer *renderer);

    SDL_Color mColor = BLACK;

    std::shared_ptr<UIComponent> mPos;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class ClickRenderTest : public TestBase {
   public:
    ClickRenderTest(Rect r, int e);

   private:
    void init();

    void onClick(Event::MouseButton b, bool clicked);

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

   protected:
    virtual void init();

    void onClick(Event::MouseButton b, bool clicked, bool red);

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

    MouseObservable::Lock mMouseLock;
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

   private:
    void init();

    bool onTimer(Timer &timer);

    void onTimerUpdate(Time dt, Timer &t);

    TimerObservable::SubscriptionPtr mTimerSub;
};

class ResizeTest : public TestBase {
   public:
    ResizeTest();

   private:
    void init();

    void onResize(ResizeData data);

    ResizeObservable::SubscriptionPtr mResizeSub;
};

class HoverTest : public TestBase {
   public:
    HoverTest(Rect r, int e);

   private:
    void init();

    void onMouseEnter();
    void onMouseLeave();
    void onHover(SDL_Point mouse);

    HoverObservable::SubscriptionPtr mHoverSub;
};

class NoMouseTest : public TestBase {
   public:
    NoMouseTest(Rect r);

    SDL_Color getColor() const;
};

class ScrollTest : public TestBase {
    class ScrollBox : public TestBase {
        friend class ScrollTest;

       public:
        ScrollBox(Rect r, int e);
    };

    typedef std::unique_ptr<ScrollBox> ScrollBoxPtr;

   public:
    using TestBase::TestBase;

   private:
    void init();

    void onRender(SDL_Renderer *r);

    void onScroll(int scroll);

    ScrollBoxPtr mBox;
    ScrollObservable::SubscriptionPtr mScrollSub;
};

class KeyTest : public TestBase {
   public:
    using TestBase::TestBase;

   private:
    void init();

    void onAnyKeyPressed(Event::KeyButton k);
    void onSpaceHeld(Event::KeyButton k);
    void onSpaceReleased(Event::KeyButton k);

    KeyboardObservable::SubscriptionPtr mAllKeysPressSub, mSpaceHeldReleaseSub;
};

class TypingTest : public TestBase {
   public:
    using TestBase::TestBase;

   private:
    void init();

    void onClick(Event::MouseButton b, bool clicked);

    void onInput(const std::string &text);

    MouseObservable::SubscriptionPtr mMouseSub;
    TypingObservable::SubscriptionPtr mTypingSub, mTypingSub2;
};

std::shared_ptr<TestBase> randomTestComponent(int w, int h);
