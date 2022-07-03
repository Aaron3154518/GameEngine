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

            window = SDL_CreateWindow("Service System Test",
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

    // Component tests
    std::unique_ptr<ClickRenderTest> t1 = ComponentFactory<ClickRenderTest>::New(Rect(25, 25, 200, 450), 1);
    std::unique_ptr<ChangeSubTest> t2 = ComponentFactory<ChangeSubTest>::New(Rect(50, 50, 400, 50), 3);
    std::unique_ptr<VisibilityTest> t3 = ComponentFactory<VisibilityTest>::New(Rect(230, 280, 40, 40), 10);
    std::unique_ptr<InheritanceTestBase> t4 = ComponentFactory<InheritanceTestBase>::New(Rect(230, 330, 40, 40), 5);
    std::unique_ptr<ClickRenderTest> t5 = ComponentFactory<ClickRenderTest>::New(Rect(275, 25, 200, 450), 3);
    std::unique_ptr<UnsubTest> t6 = ComponentFactory<UnsubTest>::New(Rect(50, 400, 400, 50), 2);
    std::unique_ptr<UpdateTest> t7 = ComponentFactory<UpdateTest>::New(Rect(230, 180, 40, 40), 10);
    std::unique_ptr<InheritanceTestDerived> t8 = ComponentFactory<InheritanceTestDerived>::New(Rect(230, 130, 40, 40), 5);
    std::unique_ptr<MultiUnsubTest> t9 = ComponentFactory<MultiUnsubTest>::New(Rect(230, 230, 40, 40), 9);

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

        ServiceSystem::Get<UpdateService>()->Get<UpdateObservable>()->next(dt);
        ServiceSystem::Get<EventService>()->Get<EventObservable>()->next(e);

        // Rendering
        SDL_RenderClear(renderer);

        ServiceSystem::Get<RenderService>()->Get<RenderOrderObservable>()->next();
        ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->next(renderer);

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