#include "RenderSystem.h"

namespace RenderSystem {

// Don't forget to update me in Renderer.h as well
void initRenderSystem(const Options &options) {
    static WindowPtr WINDOW = WindowPtr(NULL, SDL_DestroyWindow);
    if (!SDL_WasInit(SDL_INIT_EVERYTHING)) {
        int flags = SDL_WINDOW_RESIZABLE;
        if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
            std::cout << "SDL Initialized" << std::endl;

            // Use opengl
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

            WINDOW.reset(SDL_CreateWindow(options.title.c_str(),
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          options.width, options.height, flags));

            Renderer::init(WINDOW.get());
        } else {
            throw InitException();
        }
    }

    if (!TTF_WasInit()) {
        // Initialize SDL fonts module
        if (TTF_Init() == 0) {
            std::cerr << "SDL_TTF Initialized" << std::endl;
        } else {
            throw InitException();
        }
    }

    // Load options
    if (options.maximize) {
        SDL_MaximizeWindow(WINDOW.get());
    }

    AssetManager::setDefaultTexture(options.defaultTexture);
}

void teardownRenderSystem() {
    if (SDL_WasInit(SDL_INIT_EVERYTHING)) {
        SDL_Quit();
        std::cerr << "SDL Uninitialized" << std::endl;
    }
    if (TTF_WasInit()) {
        TTF_Quit();
        std::cerr << "SDL_TTF Uninitialized" << std::endl;
    }
}

void clearScreen(SDL_Color bkgrnd) {
    SDL_Color tmp;
    SDL_GetRenderDrawColor(Renderer::get(), &tmp.r, &tmp.g, &tmp.b, &tmp.a);
    SDL_SetRenderDrawColor(Renderer::get(), bkgrnd.r, bkgrnd.g, bkgrnd.b, bkgrnd.a);
    SDL_RenderClear(Renderer::get());
    SDL_SetRenderDrawColor(Renderer::get(), tmp.r, tmp.g, tmp.b, tmp.a);
}

void presentScreen() {
    SDL_RenderPresent(Renderer::get());
}

void enforceFPS(int fps) {
    static Uint32 time = 0;
    int delay = 1000 / fps;
    Uint32 dt = SDL_GetTicks() - time;
    if (dt < delay) {
        SDL_Delay(delay - dt);
    }
    time = SDL_GetTicks();
}

}  // namespace RenderSystem
