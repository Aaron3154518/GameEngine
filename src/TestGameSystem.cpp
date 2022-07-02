#define SDL_MAIN_HANDLED

#include "GameSystem.h"

int main(int argc, char* argv[]) {
    GameSystem::Init();

    GameSystem::Run();

    GameSystem::Clean();

    return 0;
}