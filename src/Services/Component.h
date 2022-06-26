#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>

#include "../Utils/Observable/Observable.h"

// Forward declaration
struct GameStruct;

class Component {
    friend class Game;

   public:
    virtual ~Component();

   protected:
    virtual void init(GameStruct &gs);

    Unsubscriber unsub;
};

#endif