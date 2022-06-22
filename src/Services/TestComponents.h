#include "../EventSystem/Event.h"
#include "../EventSystem/Time.h"
#include "../Utils/Rect/Rect.h"
#include "Component.h"
#include "CoreServices/EventService.h"
#include "CoreServices/RenderService.h"
#include "CoreServices/UpdateService.h"
#include "GameStruct.h"
#include "MouseService/MouseService.h"
#include "ServiceHandler.h"

constexpr SDL_Color WHITE{255, 255, 255, 255};
constexpr SDL_Color BLACK{0, 0, 0, 255};
constexpr SDL_Color GREEN{0, 255, 0, 255};
constexpr SDL_Color RED{255, 0, 0, 255};
constexpr SDL_Color BLUE{0, 0, 255, 255};
constexpr SDL_Color YELLOW{255, 255, 0, 255};
constexpr SDL_Color ORANGE{255, 191, 0, 255};
constexpr SDL_Color PURPLE{255, 0, 255, 255};

class TestBase : public Component {
   public:
    TestBase(Rect r, int e);

    virtual ~TestBase() = default;

    const Rect &getRect() const;

    int getElevation() const;

    virtual SDL_Color getColor() const;

   protected:
    virtual void init(GameStruct &gs);

    void onRender(SDL_Renderer *renderer);

    std::shared_ptr<UIComponent> mPos;
};

class ClickRenderTest : public TestBase {
   public:
    ClickRenderTest(Rect r, int e);

    SDL_Color getColor() const;

   private:
    void init(GameStruct &gs);

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
    void init(GameStruct &gs);

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
    void init(GameStruct &gs);

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
    void init(GameStruct &gs);

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
    void init(GameStruct &gs);

    void onClick(Event::MouseButton b, bool clicked);

    void onUpdate(Time dt);

    int delayMs = 0;
    UpdateObservable::SubscriptionPtr mUpdateSub;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

std::shared_ptr<TestBase> randomTestComponent(int w, int h);
