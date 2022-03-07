#define SDL_MAIN_HANDLED

#include <iostream>

#include "EventSystem.h"

constexpr SDL_Color WHITE{ 255,255,255,255 };
constexpr SDL_Color GREEN{ 0,255,0,255 };
constexpr SDL_Color RED{ 255,0,0,255 };
constexpr SDL_Color BLUE{ 0,0,255,255 };
constexpr SDL_Color YELLOW{ 255,255,0,255 };
constexpr SDL_Color PURPLE{ 255,0,255,255 };

int main(int argc, char* argv[]) {
	const int w = 500, h = 500;

	SDL_Window* window;
	SDL_Renderer* renderer;
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

	/*if (!TTF_WasInit()) {
		// Initialize SDL fonts module
		if (TTF_Init() == 0) {
			std::cerr << "SDL_TTF Initialized" << std::endl;
		} else {
			return 1;
		}
	}*/

	int mouseW = w / 12;
	std::array<Event::Mouse, 3> mice = {
		Event::Mouse::LEFT, Event::Mouse::MIDDLE, Event::Mouse::RIGHT};
	std::array<bool, 3 * 3> miceStatus = { false };

	int delay = 1000 / 60;
	Uint32 time = SDL_GetTicks();

	Event e;

	while (true) {
		Uint32 dt = SDL_GetTicks() - time;
		time = SDL_GetTicks();

		e.update(Time(dt));
		if (e.quit) { break; }

		for (auto& m : mice) {
			if (e[m].pressed()) {
				miceStatus[m * 3] = !miceStatus[m * 3];
			}
			if (e[m].released()) {
				miceStatus[m * 3 + 1] = !miceStatus[m * 3 + 1];
			}
			if (e[m].clicked()) {
				miceStatus[m * 3 + 2] = !miceStatus[m * 3 + 2];
			}
		}

		// Rendering
		SDL_RenderClear(renderer);

		SDL_Rect r{ 0, 0, mouseW, mouseW };
		for (auto& m : mice) {
			SDL_Color c = miceStatus[m * 3] ? BLUE : YELLOW;
			SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
			SDL_RenderFillRect(renderer, &r);
			r.x += mouseW;
			c = miceStatus[m * 3 + 1] ? BLUE : YELLOW;
			SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
			SDL_RenderFillRect(renderer, &r);
			r.x += mouseW;
			c = e[m].held() ? GREEN : WHITE;
			SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
			SDL_RenderFillRect(renderer, &r);
			r.x += mouseW;
			c = miceStatus[m * 3 + 2] ? PURPLE : RED;
			SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
			SDL_RenderFillRect(renderer, &r);
			r.x += mouseW;
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		SDL_RenderPresent(renderer);
		
		if (dt < delay) {
			SDL_Delay(delay - dt);
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}