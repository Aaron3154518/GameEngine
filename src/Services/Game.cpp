#include "Game.h"

bool Game::initialized = false;
std::vector<Component *> Game::toInit;

GameStruct &Game::gameStruct()
{
    static GameStruct GAME_STRUCT;
    return GAME_STRUCT;
}

void Game::registerComponent(Component *comp)
{
    if (initialized)
    {
        comp->init(gameStruct());
    }
    else
    {
        toInit.push_back(comp);
    }
}

void Game::init()
{
    if (initialized)
    {
        return;
    }

    std::cerr << "Initialize Game" << std::endl;

    // GameStruct must be initialized before initialized=true
    GameStruct &gs = gameStruct();

    // Set initialized to true so that any new components automatically get initialized
    initialized = true;

    for (Component *comp : toInit)
    {
        comp->init(gs);
    }
    toInit.clear();
}
