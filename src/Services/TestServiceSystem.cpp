#define SDL_MAIN_HANDLED

#include <SDL_ttf.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>

#include "../EventSystem/Event.h"
#include "Component.h"
#include "Game.h"

constexpr SDL_Color WHITE{255, 255, 255, 255};
constexpr SDL_Color BLACK{0, 0, 0, 255};
constexpr SDL_Color GREEN{0, 255, 0, 255};
constexpr SDL_Color RED{255, 0, 0, 255};
constexpr SDL_Color BLUE{0, 0, 255, 255};
constexpr SDL_Color YELLOW{255, 255, 0, 255};
constexpr SDL_Color PURPLE{255, 0, 255, 255};

class TestComponent : public Component {
   public:
    TestComponent(Rect r, int e, bool _type) : mPos(std::make_shared<UIComponent>(r, e)), type(_type) {
        Game::registerComponent(this);
    }

    const Rect &getRect() const {
        return mPos->rect;
    }

    int getElevation() const {
        return mPos->elevation;
    }

    SDL_Color getColor() const {
        return type ? (color ? GREEN : RED) : (color ? BLUE : YELLOW);
    }

   private:
    void init(GameStruct &gs) {
        mMouseSub = gs.mServices.mouseService.mouse$.subscribe(
            std::bind(type ? &onClick1 : &onClick2, this, std::placeholders::_1, std::placeholders::_2), mPos);
        gs.mServices.renderService.render$.subscribe(
            std::bind(&onRender, this, std::placeholders::_1), mPos);
    }

    void onRender(SDL_Renderer *renderer) {
        SDL_Color c = getColor();
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(renderer, &mPos->rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &mPos->rect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }

    void onClick1(Event::MouseButton b, bool clicked) {
        color = clicked;
    }

    void onClick2(Event::MouseButton b, bool clicked) {
        if (clicked) {
            color = !color;
        }
    }

    bool type = false;
    bool color = false;
    std::shared_ptr<UIComponent> mPos;
    MouseObservable::SubscriptionPtr mMouseSub;
};
typedef std::unique_ptr<TestComponent> TestComponentPtr;

bool compareTC(const TestComponentPtr &lhs, const TestComponentPtr &rhs) {
    return lhs->getElevation() <= rhs->getElevation();
}

TestComponentPtr randomTestComponent(int w, int h) {
    Rect r;
    r.x = rand() % w - 10;
    r.y = rand() % h - 10;
    r.w = rand() % (w - r.x);
    r.h = rand() % (h - r.y);
    return std::make_unique<TestComponent>(r, rand() % 20 - 10, rand() % 2 == 0);
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

    // std::vector<TestComponentPtr> components;

    // Test before init
    /*for (int i = 0; i < 5; i++) {
        components.push_back(randomTestComponent(w, h));
    }*/
    TestComponent t1(Rect(25, 25, 200, 450), 1, true);
    TestComponent t2(Rect(50, 50, 400, 50), 3, false);

    Game::init();

    // Test after init
    /*for (int i = 5; i < 10; i++) {
        components.push_back(randomTestComponent(w, h));
    }*/
    TestComponent t3(Rect(275, 25, 200, 450), 3, true);
    TestComponent t4(Rect(50, 400, 400, 50), 2, false);

    // std::sort(components.begin(), components.end(), compareTC);

    Event e;

    int delay = 1000 / 60;
    Uint32 time = SDL_GetTicks();
    Time dt;
    while (true) {
        e.update();
        if (e.quit()) {
            break;
        }

        Game::gameStruct().mServices.eventService.event$.next(e);
        Game::gameStruct().mServices.updateService.update$.next(dt);

        // Rendering
        SDL_RenderClear(renderer);

        Game::gameStruct().mServices.renderService.render$.next(renderer);

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