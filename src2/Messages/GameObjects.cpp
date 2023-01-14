#include "GameObjects.h"

// GameObjects
GameObjects::SingletonMap& GameObjects::GetSingletons() {
    static SingletonMap S_MAP;
    return S_MAP;
}

GameObjects::ObjectMap& GameObjects::GetObjects() {
    static ObjectMap O_MAP;
    return O_MAP;
}
