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
    CircleShape screenCd = CircleShape(CYAN)
                               .setCenter({halfScreenW, halfScreenW})
                               .setRadius(halfScreenW, -5)
                               .setAngleDeg(-90, 495)
                               .setDashed(100);

    RenderData image = RenderData()
                           .set("res/wizards/Catalyst.png")
                           .setDest(Rect(125, 100, 250, 250));

    RenderData pp = RenderData().setDest(image.getDest());
    TextData ppText;
    ppText.font = AssetManager::getFont(FontData{-1, 25, "|"});
    ppText.text =
        "According to all known laws of aviation, there is no way a bee should "
        "be able to fly.\n"
        "Its wings are too small to get its fat little body off the "
        "ground.\nThe bee, of course, flies anyway"
        "because bees don't care what humans think is impossible.";
    ppText.color = RED;
    ppText.w = pp.getRect().w();
    ppText.autoFit = false;
    ppText.align = Rect::Align::TOP_LEFT;
    pp.set(ppText);

    ProgressBar pb = ProgressBar().set(RED, GRAY).set(Rect(100, 400, 300, 50));
    Uint32 pbVal = 0;

    int timerVal = 1;
    TextData timerText;
    timerText.font = AssetManager::getFont(FontData{-1, 25, "|"});
    timerText.color = BLUE;
    timerText.text = std::to_string(timerVal);
    RenderData timer =
        RenderData().set(timerText).setDest(Rect(250, 450, 0, 50));

    Rect shapesDest(0, 0, pb.dest.h(), pb.dest.h());
    shapesDest.setPos(pb.dest.x(), pb.dest.cY(), Rect::Align::CENTER);
    TextureBuilder shapesTex(shapesDest.w(), shapesDest.h());
    RenderData shapes = RenderData()
                            .set(shapesTex.getTexture())
                            .setDest(shapesDest)
                            .setRotationDeg(45);

    int halfW = shapesDest.w() / 2;
    int w = halfW * sqrt(2) * .9;
    Rect rdR(0, 0, w, w);
    rdR.setPos(halfW, halfW, Rect::Align::CENTER);
    shapesTex.draw(
        CircleShape(BLUE).setCenter({halfW, halfW}).setRadius(halfW));
    shapesTex.draw(RectShape(GREEN).set(rdR));

    RenderData anim = RenderData()
                          .set("res/wizards/wizard_ss.png", 5)
                          .setDest(Rect(200, 525, 100, 100))
                          .setRotationRad(M_PI * 8 / 7);
    Uint32 animTimer = 0;

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

        screen.draw(anim);

        RenderSystem::presentScreen();

        // Updating
        Uint32 dt = SDL_GetTicks() - time;
        time += dt;

        pbVal += dt;
        while (pbVal > 1000) {
            pbVal -= 1000;

            SDL_Point dim = RenderSystem::getWindowSize();

            timerVal *= 2;
            timerText.text = std::to_string(dim.x) + "x" +
                             std::to_string(dim.y) + " - " +
                             std::to_string(timerVal);
            timer.set(timerText);

            switch (ppText.align) {
                case Rect::Align::TOP_LEFT:
                    ppText.align = Rect::Align::CENTER;
                    break;
                case Rect::Align::CENTER:
                    ppText.align = Rect::Align::BOT_RIGHT;
                    break;
                default:
                    ppText.align = Rect::Align::TOP_LEFT;
                    break;
            }
            pp.set(ppText);

            image.set(pbVal % 2 == 0 ? "res/wizards/Catalyst.png" : "oops");
        }

        animTimer += dt;
        while (animTimer > 150) {
            animTimer -= 150;
            anim.nextFrame();
        }

        // FPS
        RenderSystem::enforceFPS(60);
    }

    std::cerr << "Avg: " << ((float)sum / cnt) << "ms" << std::endl;

    RenderSystem::teardownRenderSystem();

    return 0;
}