#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <vector>

#include "GameStruct.h"
#include "Component.h"

class Game
{
public:
    static const Game &Get();

    static void registerComponent(Component *comp);
    static void unregisterComponent(Component *comp);

    void init();

private:
    Game() = default;
    ~Game() = default;

    static Game game;

    std::vector<Component *> mComponents;
    GameStruct mGameStruct;
    bool initialized = false;
};

#endif
