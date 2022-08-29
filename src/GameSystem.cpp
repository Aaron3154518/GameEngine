#include "GameSystem.h"

namespace GameSystem {

void Init(RenderSystem::Options options) {
    RenderSystem::initRenderSystem(options);
}

void Run() {
    Event e;

    float mspfSum = 0;
    Uint32 mspfT1, mspfCnt = 0;

    /** Game Loop:
     * 1) Interperet events
     * 2) Compute component under mouse
     * 3) Trigger update
     * 4) Handle events
     * 5) Lock render order
     * 6) Perform render
     */
    while (true) {
        mspfT1 = SDL_GetTicks();

        // Check events
        e.update();
        // Check quit
        if (e.quit()) {
            break;
        }

        // Compute the current component under the mouse
        ServiceSystem::Get<RenderService, RenderOrderObservable>()
            ->computeUnderMouse(e.mouse());

        // Send update
        ServiceSystem::Get<UpdateService, UpdateObservable>()->next(e.dt());
        // Send event
        ServiceSystem::Get<EventService, EventObservable>()->next(e);

        // Clear screen
        RenderSystem::clearScreen(LGRAY);

        // Send renderer
        ServiceSystem::Get<RenderService, RenderOrderObservable>()->next();
        ServiceSystem::Get<RenderService, RenderObservable>()->next(
            Renderer::get());

        // Display screen
        RenderSystem::presentScreen();

        mspfSum += SDL_GetTicks() - mspfT1;
        mspfCnt++;

        // FPS
        RenderSystem::enforceFPS(60);
    }

    std::cerr << "Average milliseconds per frame: "
              << (mspfCnt == 0 ? 0 : mspfSum / mspfCnt) << "ms" << std::endl;
}

void Clean() { RenderSystem::teardownRenderSystem(); }

}  // namespace GameSystem