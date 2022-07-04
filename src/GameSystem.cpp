#include "GameSystem.h"

namespace GameSystem {

void Init(RenderSystem::Options options) {
    RenderSystem::initRenderSystem(options);
}

void Run() {
    Event e;

    while (true) {
        // Check events
        e.update();
        // Check quit
        if (e.quit()) {
            break;
        }

        // Send update
        ServiceSystem::Get<UpdateService, UpdateObservable>()->next(e.dt());
        // Send event
        ServiceSystem::Get<EventService, EventObservable>()->next(e);

        // Clear screen
        RenderSystem::clearScreen(LGRAY);

        // Send render order
        ServiceSystem::Get<RenderService, RenderOrderObservable>()->next();
        // Send renderer
        ServiceSystem::Get<RenderService, RenderObservable>()->next(Renderer::get());

        // Display screen
        RenderSystem::presentScreen();

        // FPS
        RenderSystem::enforceFPS(60);
    }
}

void Clean() {
    RenderSystem::teardownRenderSystem();
}

}  // namespace GameSystem