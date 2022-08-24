#define SDL_MAIN_HANDLED

#include <RenderSystem/RenderSystem.h>

#include <ctime>

int main(int argc, char *argv[]) {
    RenderSystem::Options options;
    options.title = "Render System Test";
    options.width = options.height = 750;
    options.defaultTexture = "res/default.png";

    RenderSystem::initRenderSystem(options);

    TextureBuilder screen;

    const int halfScreenW = options.width / 2;
    CircleData screenCd;
    screenCd.color = CYAN;
    screenCd.set({halfScreenW, halfScreenW}, halfScreenW, -10)
        .setAngleDeg(-90, 480)
        .setDashed(100);

    RenderData image;
    image.texture = AssetManager::getTexture("res/wizards/Catalyst.png");
    image.dest = Rect(125, 100, 250, 250);
    image.shrinkToTexture();

    TextRenderData pp;
    pp.dest = image.dest;
    pp.tData.font = AssetManager::getFont(FontData{-1, 25, "|"});
    pp.tData.text =
        "According to all known laws of aviation, there is no way a bee should "
        "be able to fly.\n"
        "Its wings are too small to get its fat little body off the "
        "ground.\nThe bee, of course, flies anyway"
        "because bees don't care what humans think is impossible.";
    pp.tData.color = RED;
    pp.tData.w = pp.dest.w();
    pp.tData.autoFit = false;
    pp.tData.align = Rect::Align::TOP_LEFT;
    pp.renderText();
    pp.shrinkToTexture();

    ProgressBar pb;
    pb.set(RED, GRAY).set(Rect(100, 400, 300, 50));
    float pbVal = 0.;

    int timerVal = 1;
    TextRenderData timer;
    timer.dest = Rect(250, 450, 0, 50);
    timer.tData.font = AssetManager::getFont(FontData{-1, 25, "|"});
    timer.tData.color = BLUE;
    timer.tData.text = std::to_string(timerVal);
    timer.renderText();
    timer.shrinkToTexture(0, timer.dest.h());

    RenderData shapes;
    shapes.dest = Rect(0, 0, pb.rect.h(), pb.rect.h());
    shapes.dest.setPos(pb.rect.x(), pb.rect.cY(), Rect::Align::CENTER);
    TextureBuilder shapesTex(shapes.dest.w(), shapes.dest.h());
    shapes.texture = shapesTex.getTexture();

    CircleData cd;
    cd.color = BLUE;
    int halfW = shapes.dest.w() / 2;
    shapesTex.draw(cd.set(SDL_Point{halfW, halfW}, halfW));
    RectData rd;
    rd.color = GREEN;
    int w = cd.r2 * sqrt(2) * .9;
    Rect rdR(0, 0, w, w);
    rdR.setPos(halfW, halfW, Rect::Align::CENTER);
    shapesTex.draw(rd.set(rdR, 3));

    Uint32 time = SDL_GetTicks();

    Uint32 t1, sum = 0, cnt = 0;

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
        RenderSystem::clearScreen(LGRAY);

        t1 = SDL_GetTicks();
        screen.draw(screenCd);
        sum += SDL_GetTicks() - t1;
        cnt++;

        screen.draw(pb.set(pbVal, 1000));
        screen.draw(shapes);

        screen.draw(timer);

        screen.draw(image);
        screen.draw(pp);

        RenderSystem::presentScreen();

        // Updating
        Uint32 dt = SDL_GetTicks() - time;
        time += dt;

        pbVal += dt;
        while (pbVal > 1000.) {
            pbVal -= 1000.;

            SDL_Point dim = RenderSystem::getWindowSize();

            timerVal *= 2;
            timer.tData.text = std::to_string(dim.x) + "x" +
                               std::to_string(dim.y) + " - " +
                               std::to_string(timerVal);
            timer.renderText();
            timer.shrinkToTexture(0, timer.dest.h());

            switch (pp.tData.align) {
                case Rect::Align::TOP_LEFT:
                    pp.tData.align = Rect::Align::CENTER;
                    break;
                case Rect::Align::CENTER:
                    pp.tData.align = Rect::Align::BOT_RIGHT;
                    break;
                default:
                    pp.tData.align = Rect::Align::TOP_LEFT;
                    break;
            }
            pp.renderText();
            pp.shrinkToTexture();

            image.texture = AssetManager::getTexture(
                (int)pbVal % 2 == 0 ? "res/wizards/Catalyst.png" : "oops");
            image.dest = Rect(125, 100, 250, 250);
            image.shrinkToTexture();
        }

        // FPS
        RenderSystem::enforceFPS(60);
    }

    std::cerr << "Avg: " << ((float)sum / cnt) << "ms" << std::endl;

    RenderSystem::teardownRenderSystem();

    return 0;
}