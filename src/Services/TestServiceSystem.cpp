#define SDL_MAIN_HANDLED

#include <iostream>
#include <sstream>

#include <SDL_ttf.h>

#include "Game.h"

constexpr SDL_Color WHITE{255, 255, 255, 255};
constexpr SDL_Color BLACK{0, 0, 0, 255};
constexpr SDL_Color GREEN{0, 255, 0, 255};
constexpr SDL_Color RED{255, 0, 0, 255};
constexpr SDL_Color BLUE{0, 0, 255, 255};
constexpr SDL_Color YELLOW{255, 255, 0, 255};
constexpr SDL_Color PURPLE{255, 0, 255, 255};

int main(int argc, char *argv[])
{
	const int w = 500, h = 500;

	SDL_Window *window;
	SDL_Renderer *renderer;
	if (!SDL_WasInit(SDL_INIT_EVERYTHING))
	{
		int flags = SDL_WINDOW_RESIZABLE;
		if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
		{
			std::cout << "SDL Initialized" << std::endl;

			// Use opengl
			SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

			window = SDL_CreateWindow("Event System Test",
									  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
									  w, h, flags);

			renderer = SDL_CreateRenderer(window, -1, 0);
		}
		else
		{
			return 1;
		}
	}

	if (!TTF_WasInit())
	{
		// Initialize SDL fonts module
		if (TTF_Init() == 0)
		{
			std::cerr << "SDL_TTF Initialized" << std::endl;
		}
		else
		{
			return 1;
		}
	}

	int textH = w / 12;

	TTF_Font *font = TTF_OpenFont("res/fonts/times.ttf", textH);

	Event e;

	int delay = 1000 / 60;
	Uint32 time = SDL_GetTicks();
	while (true)
	{
		e.update();
		if (e.quit())
		{
			break;
		}
		if (e.resized())
		{
			textH = fmin(e.newW(), e.newH()) / 12;
			TTF_CloseFont(font);
			font = TTF_OpenFont("res/fonts/times.ttf", textH);
		}

		// Rendering
		SDL_RenderClear(renderer);

		// Mouse
		SDL_Rect r{0, 0, textH, textH};
		SDL_Surface *surf = TTF_RenderText_Solid(font, "Hello World", BLACK);
		SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_RenderCopy(renderer, tex, NULL, &r);
		SDL_FreeSurface(surf);
		SDL_DestroyTexture(tex);

		SDL_RenderPresent(renderer);

		uint32_t dt = SDL_GetTicks() - time;
		if (dt < delay)
		{
			SDL_Delay(delay - dt);
		}
		time = SDL_GetTicks();
	}

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}