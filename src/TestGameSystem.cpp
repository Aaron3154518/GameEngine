#define SDL_MAIN_HANDLED

#include <GameSystem.h>

int main(int argc, char* argv[]) {
    RenderSystem::Options options;
    options.title = "Game System Test";
    options.maximize = true;

    GameSystem::Init(options);

    GameSystem::Run();

    GameSystem::Clean();

    return 0;
}