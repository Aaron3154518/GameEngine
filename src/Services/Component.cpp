#include "Component.h"

#include "Game.h"

Component::~Component() {
    unsub.unsubscribe();
}

void Component::init(GameStruct &gs) {}