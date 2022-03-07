#define SDL_MAIN_HANDLED

#include "RenderSystem.h"

int main(int argc, char* argv[]) {
	initRenderSystem(500, 500, "Render System Test");

	TextureBuilder screen;

	ProgressBar pb;
	pb.set(RED, BLACK).set(Rect(100, 400, 200, 50));
	double pbVal = 0.;

	bool running = true;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
				break;
			}
		}
		SDL_RenderClear(Renderer::get());

		pb.set(pbVal, 100.);
		screen.drawProgressBar(pb);

		SDL_RenderPresent(Renderer::get());

		pbVal = fmod((pbVal + 1), 100);
	}

	teardownRenderSystem();

	return 0;
}