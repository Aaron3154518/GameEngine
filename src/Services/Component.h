#ifndef COMPONENT_H
#define COMPONENT_H

// Forward declaration
struct GameStruct;

class Component
{
    friend class Game;

public:
    Component();
    ~Component();

protected:
    virtual void init(GameStruct &g) {}
};

#endif