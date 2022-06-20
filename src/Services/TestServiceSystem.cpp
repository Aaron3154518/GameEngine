#define SDL_MAIN_HANDLED

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>

#include <SDL_ttf.h>

#include "Component.h"
#include "../EventSystem/Event.h"
#include "Game.h"

constexpr SDL_Color WHITE{255, 255, 255, 255};
constexpr SDL_Color BLACK{0, 0, 0, 255};
constexpr SDL_Color GREEN{0, 255, 0, 255};
constexpr SDL_Color RED{255, 0, 0, 255};
constexpr SDL_Color BLUE{0, 0, 255, 255};
constexpr SDL_Color YELLOW{255, 255, 0, 255};
constexpr SDL_Color PURPLE{255, 0, 255, 255};

class TestComponent : public Component
{
public:
	TestComponent(Rect r, int e) : Component(), mPos(std::make_shared<UIComponent>(r, e)) {}

	void init(GameStruct &gs)
	{
		std::function<void(Event::MouseButton, bool)> func =
			std::bind(onClick, this, std::placeholders::_1, std::placeholders::_2);
		mMouseSub = gs.mServices.mouseService.mouse$.subscribe(func, mPos);
	}

	const Rect &getRect() const
	{
		return mPos->rect;
	}

	int getElevation() const
	{
		return mPos->elevation;
	}

	SDL_Color getColor() const
	{
		return color ? GREEN : RED;
	}

private:
	void onClick(Event::MouseButton b, bool clicked)
	{
		color = clicked;
	}

	bool color = false;
	std::shared_ptr<UIComponent> mPos;
	MouseObservable::SubscriptionPtr mMouseSub;
};
typedef std::unique_ptr<TestComponent> TestComponentPtr;

bool compareTC(const TestComponentPtr &lhs, const TestComponentPtr &rhs)
{
	return lhs->getElevation() <= rhs->getElevation();
}

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

	std::vector<TestComponentPtr> components;
	// Test before init
	for (int i = 0; i < 5; i++)
	{
		Rect r(rand() % w, rand() % h, rand() % w, rand() % h);
		components.push_back(std::make_unique<TestComponent>(r, rand() % 100));
	}

	Game::init();

	// Test after init
	for (int i = 5; i < 10; i++)
	{
		Rect r(rand() % w, rand() % h, rand() % w, rand() % h);
		// TestComponent tmp(r, rand() % 20 - 10);
		// components.push_back(tmp);
	}

	std::sort(components.begin(), components.end(), compareTC);

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
		}
		// std::cerr << "Next" << std::endl;
		Game::getGameStruct().mServices.eventService.event$.next(e);

		// Rendering
		SDL_RenderClear(renderer);

		// Draw test components
		for (auto &component : components)
		{
			SDL_Color c = component->getColor();
			SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
			SDL_RenderFillRect(renderer, &component->getRect());
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &component->getRect());
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		SDL_RenderPresent(renderer);

		uint32_t dt = SDL_GetTicks() - time;
		if (dt < delay)
		{
			SDL_Delay(delay - dt);
		}
		time = SDL_GetTicks();
	}

	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}