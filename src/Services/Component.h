#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>

// Forward declaration
struct GameStruct;

class Component
{
    friend class Game;

public:
    Component();
    Component(const Component &c);
    ~Component();

protected:
    virtual void init(GameStruct &gs) {}
};

#endif