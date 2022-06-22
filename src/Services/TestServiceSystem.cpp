#define SDL_MAIN_HANDLED

#include <SDL_ttf.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>

#include "../EventSystem/Event.h"
#include "../EventSystem/Time.h"
#include "../Utils/Rect/Rect.h"
#include "Component.h"
#include "CoreServices/EventService.h"
#include "CoreServices/RenderService.h"
#include "CoreServices/UpdateService.h"
#include "Game.h"
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
    TestBase(Rect r, int e) : mPos(std::make_shared<UIComponent>(r, e)) {}

    virtual ~TestBase() = default;

    const Rect &getRect() const {
        return mPos->rect;
    }

    int getElevation() const {
        return mPos->elevation;
    }

    virtual SDL_Color getColor() const {
        return BLACK;
    }

   protected:
    virtual void init(GameStruct &gs) {}

    void onRender(SDL_Renderer *renderer) {
        SDL_Color c = getColor();
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(renderer, &mPos->rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &mPos->rect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }

    std::shared_ptr<UIComponent> mPos;
};

class ClickRenderTest : public TestBase {
   public:
    ClickRenderTest(Rect r, int e) : TestBase(r, e) {
        Game::registerComponent(this);
    }

    SDL_Color getColor() const {
        return color ? GREEN : RED;
    }

   private:
    void init(GameStruct &gs) {
        mMouseSub = ServiceHandler::Get<MouseService>()->mouse$.subscribe(
            std::bind(&ClickRenderTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
            mPos);
        mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
            std::bind(&ClickRenderTest::onRender, this, std::placeholders::_1), mPos);
    }

    void onClick(Event::MouseButton b, bool clicked) {
        color = clicked;
    }

    bool color = false;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class ChangeSubTest : public TestBase {
   public:
    ChangeSubTest(Rect r, int e) : TestBase(r, e) {
        Game::registerComponent(this);
    }

    SDL_Color getColor() const {
        return color ? YELLOW : BLUE;
    }

   private:
    void init(GameStruct &gs) {
        mMouseSub = ServiceHandler::Get<MouseService>()->mouse$.subscribe(
            std::bind(&ChangeSubTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
            mPos);
        mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
            std::bind(&ChangeSubTest::onRender, this, std::placeholders::_1), mPos);
    }

    void onClick(Event::MouseButton b, bool clicked) {
        if (clicked) {
            color = !color;
            if (color) {
                mRenderSub->unsubscribe();
                mRenderSub.reset();
            } else {
                mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
                    std::bind(&ChangeSubTest::onRender, this, std::placeholders::_1), mPos);
            }
        }
    }

    bool color = false;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class UnsubTest : public TestBase {
   public:
    UnsubTest(Rect r, int e) : TestBase(r, e) {
        Game::registerComponent(this);
    }

    SDL_Color getColor() const {
        return color ? ORANGE : PURPLE;
    }

   private:
    void init(GameStruct &gs) {
        mMouseSub = ServiceHandler::Get<MouseService>()->mouse$.subscribe(
            std::bind(&UnsubTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
            mPos);
        mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
            std::bind(&UnsubTest::onRender, this, std::placeholders::_1), mPos);
    }

    void onClick(Event::MouseButton b, bool clicked) {
        if (clicked) {
            color = !color;
            if (color) {
                mRenderSub->changeSubscription([](SDL_Renderer *) {});
            } else {
                mRenderSub->changeSubscription(std::bind(&UnsubTest::onRender, this, std::placeholders::_1));
            }
        }
    }

    bool color = false;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

class VisibilityTest : public TestBase {
   public:
    VisibilityTest(Rect r, int e) : TestBase(r, e) {
        Game::registerComponent(this);
    }

   private:
    void init(GameStruct &gs) {
        mMouseSub = ServiceHandler::Get<MouseService>()->mouse$.subscribe(
            std::bind(&VisibilityTest::onClick, this, std::placeholders::_1, std::placeholders::_2),
            mPos);
        mRenderSub = ServiceHandler::Get<RenderService>()->render$.subscribe(
            std::bind(&VisibilityTest::onRender, this, std::placeholders::_1), mPos);
    }

    void onClick(Event::MouseButton b, bool clicked) {
        if (clicked) {
            mPos->visible = false;
            delay = Time(1000);
        }
    }

    void onUpdate(Time dt) {
        if (!mPos->visible) {
            delay -= dt;
        }
    }

    Time delay;
    UpdateObservable::SubscriptionPtr mUpdateSub;
    MouseObservable::SubscriptionPtr mMouseSub;
    RenderObservable::SubscriptionPtr mRenderSub;
};

std::shared_ptr<TestBase> randomTestComponent(int w, int h) {
    Rect r;
    r.x = rand() % w - 10;
    r.y = rand() % h - 10;
    r.w = rand() % (w - r.x);
    r.h = rand() % (h - r.y);
    int e = rand() % 20 - 10;
    switch (rand() % 3) {
        case 1:
            return std::make_shared<ChangeSubTest>(r, e);
        case 2:
            return std::make_shared<UnsubTest>(r, e);
        default:
            return std::make_shared<ClickRenderTest>(r, e);
    }
}

// TODO: Move init and game loop to dedicated classes
int main(int argc, char *argv[]) {
    const int w = 500, h = 500;

    SDL_Window *window;
    SDL_Renderer *renderer;
    if (!SDL_WasInit(SDL_INIT_EVERYTHING)) {
        int flags = SDL_WINDOW_RESIZABLE;
        if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
            std::cout << "SDL Initialized" << std::endl;

            // Use opengl
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

            window = SDL_CreateWindow("Event System Test",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      w, h, flags);

            renderer = SDL_CreateRenderer(window, -1, 0);
        } else {
            return 1;
        }
    }

    if (!TTF_WasInit()) {
        // Initialize SDL fonts module
        if (TTF_Init() == 0) {
            std::cerr << "SDL_TTF Initialized" << std::endl;
        } else {
            return 1;
        }
    }

    // Test before init
    ClickRenderTest t1(Rect(25, 25, 200, 450), 1);
    ChangeSubTest t2(Rect(50, 50, 400, 50), 3);
    VisibilityTest t5(Rect(230, 230, 40, 40), 10);

    Game::init();

    // Test after init
    ClickRenderTest t3(Rect(275, 25, 200, 450), 3);
    UnsubTest t4(Rect(50, 400, 400, 50), 2);

    Event e;

    int delay = 1000 / 60;
    Uint32 time = SDL_GetTicks();
    Time dt;
    while (true) {
        e.update();
        if (e.quit()) {
            break;
        }

        ServiceHandler::Get<EventService>()->event$.next(e);
        ServiceHandler::Get<UpdateService>()->update$.next(dt);

        // Rendering
        SDL_RenderClear(renderer);

        ServiceHandler::Get<RenderService>()->render$.next(renderer);

        SDL_RenderPresent(renderer);

        dt = Time(SDL_GetTicks() - time);
        if (dt < delay) {
            SDL_Delay(delay - dt);
        }
        time = SDL_GetTicks();
    }

    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}