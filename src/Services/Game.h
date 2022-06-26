#ifndef GAME_H
#define GAME_H

#include <algorithm>
#include <set>

#include "Component.h"
#include "GameStruct.h"

class Game {
   public:
    static GameStruct &gameStruct();

    static void registerComponent(Component *comp);

    static void init();

   private:
    Game() = delete;
    ~Game() = delete;

    static bool &initialized();

    // No duplicate inits
    static std::set<Component *> &toInit();
};

#endif
