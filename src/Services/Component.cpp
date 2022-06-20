#include "Component.h"
#include "Game.h"

Component::Component()
{
    Game::registerComponent(this);
}

Component::Component(const Component &c)
{
    Game::registerComponent(this);
}

Component::~Component()
{
    Game::unregisterComponent(this);
}