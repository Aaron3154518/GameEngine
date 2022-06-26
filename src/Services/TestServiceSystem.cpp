#define SDL_MAIN_HANDLED

#include <SDL_ttf.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>

#include "../EventSystem/Event.h"
#include "../EventSystem/Time.h"
#include "../Utils/Rect/Rect.h"
#include "Game.h"
#include "ServiceHandler.h"
#include "TestComponents.h"

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
    VisibilityTest t5(Rect(230, 280, 40, 40), 10);
    InheritanceTestBase t8(Rect(230, 330, 40, 40), 5);

    Game::init();

    // Test after init
    ClickRenderTest t3(Rect(275, 25, 200, 450), 3);
    UnsubTest t4(Rect(50, 400, 400, 50), 2);
    UpdateTest t6(Rect(230, 180, 40, 40), 10);
    InheritanceTestDerived t7(Rect(230, 130, 40, 40), 5);
    MultiUnsubTest t9(Rect(230, 230, 40, 40), 9);

    Event e;

    int delay = 1000 / 60;
    Uint32 time = SDL_GetTicks();
    Time dt;
    while (true) {
        dt = SDL_GetTicks() - time;
        time = SDL_GetTicks();

        e.update();
        if (e.quit()) {
            break;
        }

        ServiceHandler::Get<UpdateService>()->update$.next(dt);
        ServiceHandler::Get<EventService>()->event$.next(e);

        // Rendering
        SDL_RenderClear(renderer);

        ServiceHandler::Get<RenderService>()->renderOrder$.next();
        ServiceHandler::Get<RenderService>()->render$.next(renderer);

        SDL_RenderPresent(renderer);

        Uint32 loopDt = SDL_GetTicks() - time;
        if (loopDt < delay) {
            SDL_Delay(delay - loopDt);
        }
    }

    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}