#include "Component.h"
#include "Game.h"

Component::Component()
{
    Game::registerComponent(this);
}

Component::~Component()
{
    Game::unregisterComponent(this);
}