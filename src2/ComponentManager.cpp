#include "ComponentManager.h"

std::unordered_map<std::type_index, ComponentManagerBasePtr>&
ComponentService::ComponentManagers() {
    static std::unordered_map<std::type_index, ComponentManagerBasePtr>
        MANAGERS;
    return MANAGERS;
}