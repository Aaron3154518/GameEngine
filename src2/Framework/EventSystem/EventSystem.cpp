#include "EventSystem.h"

// EventSystem
Event EventSystem::mEvent;

const Event& EventSystem::get() { return mEvent; }

void EventSystem::update() { mEvent.update(); }
