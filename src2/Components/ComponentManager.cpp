#include "ComponentManager.h"

namespace Components {
// ComponentService
std::unordered_map<std::type_index, ComponentManagerBasePtr>&
ComponentService::ComponentManagers() {
    static std::unordered_map<std::type_index, ComponentManagerBasePtr>
        MANAGERS;
    return MANAGERS;
}
}  // namespace Components
