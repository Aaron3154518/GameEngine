#include "GameSystem.h"

namespace GameSystem {

void Init() {
    RenderSystem::initRenderSystem(500, 500, "Game System Test");
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
        ServiceSystem::Get<UpdateService>()->Get<UpdateObservable>()->next(e.dt());
        // Send event
        ServiceSystem::Get<EventService>()->Get<EventObservable>()->next(e);

        // Clear screen
        RenderSystem::clearScreen(LGRAY);

        // Send render order
        ServiceSystem::Get<RenderService>()->Get<RenderOrderObservable>()->next();
        // Send renderer
        ServiceSystem::Get<RenderService>()->Get<RenderObservable>()->next(Renderer::get());

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