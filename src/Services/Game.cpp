#include "Game.h"

Game Game::game;

const Game &Game::Get()
{
    return game;
}

GameStruct &Game::getGameStruct()
{
    return game.mGameStruct;
}

void Game::registerComponent(Component *comp)
{
    game.mComponents.push_back(comp);
    if (game.initialized)
    {
        comp->init(game.mGameStruct);
    }
}

void Game::unregisterComponent(Component *comp)
{
    auto it = std::find_if(game.mComponents.begin(), game.mComponents.end(), [comp](const Component *c) -> bool
                           { return c == comp; });
    if (it != game.mComponents.end())
    {
        game.mComponents.erase(it);
    }
}

void Game::init()
{
    if (game.initialized)
    {
        return;
    }

    for (Component *comp : game.mComponents)
    {
        comp->init(game.mGameStruct);
    }

    game.initialized = true;
}
