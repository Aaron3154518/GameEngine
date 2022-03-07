#define SDL_MAIN_HANDLED

#include "RenderSystem.h"

int main(int argc, char* argv[]) {
	initRenderSystem(500, 500, "Render System Test");

	AssetManager am;

	TextureBuilder screen;

	RenderData image;
	image.texture = am.getTexture("res/wizards/Catalyst.png");
	image.dest = Rect(125, 100, 250, 250);
	image.fitToTexture();

	TextRenderData pp;
	pp.dest = image.dest;
	pp.tData.font = am.getFont(FontData{ -1, 25, "|" });
	pp.tData.text = "According to all known laws of aviation, there is no way a bee should be able to fly.\n"
		"Its wings are too small to get its fat little body off the ground.\nThe bee, of course, flies anyway"
		"because bees don't care what humans think is impossible.";
	pp.tData.color = RED;
	pp.tData.w = pp.dest.w;
	pp.tData.align = Rect::Align::TOP_LEFT;
	pp.renderText();
	pp.fitToTexture();

	ProgressBar pb;
	pb.set(RED, LGRAY).set(Rect(100, 400, 300, 50));
	double pbVal = 0.;

	int timerVal = 1;
	TextRenderData timer;
	timer.dest = Rect(250, 450, 0, 50);
	timer.tData.font = am.getFont(FontData{-1, 25, "|"});
	timer.tData.color = BLUE;
	timer.tData.text = std::to_string(timerVal);
	timer.renderText();
	timer.fitToTexture(0, timer.dest.h);

	RenderData shapes;
	shapes.dest = Rect(0, 0, pb.rect.h, pb.rect.h);
	shapes.dest.setCenter(pb.rect.x, pb.rect.cY());
	TextureBuilder shapesTex(shapes.dest.w, shapes.dest.h);
	shapes.texture = shapesTex.getTexture();

	CircleData cd;
	cd.color = BLUE;
	int halfW = shapes.dest.w / 2;
	shapesTex.draw(cd.set(SDL_Point{ halfW, halfW }, halfW));
	RectData rd;
	rd.color = GREEN;
	int w = cd.r2 * sqrt(2) * .9;
	Rect rdR(0, 0, w, w);
	rdR.setCenter(halfW, halfW);
	shapesTex.draw(rd.set(rdR));

	Uint32 time = SDL_GetTicks();

	bool running = true;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
				break;
			}
		}

		// Rendering
		clearScreen(LGRAY);

		pb.set(pbVal, 1000);
		screen.draw(pb);
		screen.draw(shapes);

		screen.draw(timer);

		screen.draw(image);
		screen.draw(pp);

		presentScreen();

		// Updating
		Uint32 dt = SDL_GetTicks() - time;
		time += dt;

		pbVal += dt;
		while (pbVal > 1000.) {
			pbVal -= 1000.;

			timerVal *= 2;
			timer.tData.text = std::to_string(timerVal);
			timer.renderText();
			timer.fitToTexture(0, timer.dest.h);

			switch (pp.tData.align) {
				case Rect::Align::TOP_LEFT:
					pp.tData.align = Rect::Align::CENTER; break;
				case Rect::Align::CENTER:
					pp.tData.align = Rect::Align::BOT_RIGHT; break;
				default:
					pp.tData.align = Rect::Align::TOP_LEFT; break;
			}
			pp.renderText();
			pp.fitToTexture();
		}

		// FPS
		enforceFPS(60);
	}

	teardownRenderSystem();

	return 0;
}