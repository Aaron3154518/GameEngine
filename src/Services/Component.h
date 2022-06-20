#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>

// Forward declaration
struct GameStruct;

class Component {
    friend class Game;

   public:
    virtual ~Component();

   protected:
    virtual void init(GameStruct &gs);
};

#endif