#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <vector>

#include "GameStruct.h"
#include "Component.h"

class Game
{
public:
    static GameStruct &gameStruct();

    static void registerComponent(Component *comp);

    static void init();

private:
    Game() = delete;
    ~Game() = delete;

    static std::vector<Component *> toInit;
    static bool initialized;
};

#endif
