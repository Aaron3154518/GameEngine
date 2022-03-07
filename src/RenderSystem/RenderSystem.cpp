#include "RenderSystem.h"

void initRenderSystem(int w, int h, std::string name) {
	static WindowPtr WINDOW = WindowPtr(NULL, SDL_DestroyWindow);
	if (!SDL_WasInit(SDL_INIT_EVERYTHING)) {
		int flags = SDL_WINDOW_RESIZABLE;
		if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
			std::cout << "SDL Initialized" << std::endl;

			// Use opengl
			SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

			WINDOW.reset(SDL_CreateWindow(name.c_str(),
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				w, h, flags));

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