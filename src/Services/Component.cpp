#include "Component.h"

Component::~Component() {
    unsub.unsubscribe();
}

void Component::init() {}