#define SDL_MAIN_HANDLED

#include <SDL_ttf.h>
#include <ServiceSystem/ServiceSystem.h>
#include <ServiceSystem/TestComponents.h>
#include <Utils/Event.h>
#include <Utils/Rect.h>
#include <Utils/Time.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>

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

            window =
                SDL_CreateWindow("Service System Test", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, w, h, flags);

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

    // Component tests
    auto t1 = ComponentFactory<ClickRenderTest>::New(Rect(25, 25, 200, 450), 1);
    auto t2 = ComponentFactory<ChangeSubTest>::New(Rect(50, 50, 400, 50), 3);
    auto t3 = ComponentFactory<VisibilityTest>::New(Rect(230, 280, 40, 40), 10);
    auto t4 =
        ComponentFactory<InheritanceTestBase>::New(Rect(230, 330, 40, 40), 5);
    auto t5 =
        ComponentFactory<ClickRenderTest>::New(Rect(275, 25, 200, 450), 3);
    auto t6 = ComponentFactory<UnsubTest>::New(Rect(50, 400, 400, 50), 2);
    auto t7 = ComponentFactory<UpdateTest>::New(Rect(230, 180, 40, 40), 10);
    auto t8 = ComponentFactory<InheritanceTestDerived>::New(
        Rect(230, 130, 40, 40), 5);
    auto t9 = ComponentFactory<MultiUnsubTest>::New(Rect(230, 230, 40, 40), 9);
    auto t10 = ComponentFactory<MouseLockTest>::New(Rect(100, 225, 50, 50), 10);
    auto t11 = ComponentFactory<DragTest>::New(Rect(405, 180, 40, 40), 11, -1);
    auto t12 = ComponentFactory<DragTest>::New(Rect(405, 230, 40, 40), 11, 0);
    auto t13 =
        ComponentFactory<DragTest>::New(Rect(405, 280, 40, 40), 11, 1000);
    auto t14 = ComponentFactory<TimerTest>::New(Rect(5, 5, 40, 40), 3);
    auto t15 = ComponentFactory<ResizeTest>::New();
    auto t16 = ComponentFactory<HoverTest>::New(Rect(5, 455, 40, 40), 9);
    auto t17 = ComponentFactory<NoMouseTest>::New(Rect(300, 225, 50, 50));

    Event e;

    Uint32 delay = 1000 / 60;
    Uint32 time = SDL_GetTicks();
    Time dt;
    while (true) {
        dt = SDL_GetTicks() - time;
        time = SDL_GetTicks();

        e.update();
        if (e.quit()) {
            break;
        }

        ServiceSystem::Get<RenderService, RenderOrderObservable>()
            ->computeUnderMouse(e.mouse());

        ServiceSystem::Get<UpdateService, UpdateObservable>()->next(dt);
        ServiceSystem::Get<EventService, EventObservable>()->next(e);

        // Rendering
        SDL_RenderClear(renderer);

        ServiceSystem::Get<RenderService, RenderOrderObservable>()->next();
        ServiceSystem::Get<RenderService, RenderObservable>()->next(renderer);

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