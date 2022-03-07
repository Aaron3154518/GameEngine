#define SDL_MAIN_HANDLED

#include "RenderSystem.h"

int main(int argc, char* argv[]) {
	initRenderSystem(500, 500, "Render System Test");

	AssetManager am;

	TextureBuilder screen;

	RectData bkgrnd;
	bkgrnd.color = WHITE;

	RenderData image;
	image.texture = am.getTexture("res/wizards/Crystal.png");
	image.dest = Rect(125, 100, 250, 250);
	image.fitToTexture();

	TextRenderData pp;

	ProgressBar pb;
	pb.set(RED, LGRAY).set(Rect(100, 400, 300, 50));
	double pbVal = 0.;

	TextRenderData timer;

	Uint32 time = SDL_GetTicks();
	int delay = 1000 / 60;

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

		screen.drawRect(bkgrnd);

		pb.set(pbVal, 1000);
		screen.drawProgressBar(pb);

		screen.draw(image);

 		SDL_RenderPresent(Renderer::get());

		pbVal = fmod((pbVal + delay), 1000.);

		Uint32 dt = SDL_GetTicks() - time;
		if (dt < delay) {
			SDL_Delay(delay - dt);
		}
		time = SDL_GetTicks();
	}

	teardownRenderSystem();

	return 0;
}