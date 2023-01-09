#include "ComponentManager.h"

namespace Components {
// ComponentManagerBase
bool ComponentManagerBase::hasEntity(Entities::UUID eId) {
    return mComponents.find(eId) != mComponents.end();
}

const Component& ComponentManagerBase::operator[](Entities::UUID eId) const {
    auto it = mComponents.find(eId);
    if (it == mComponents.end()) {
        throw std::runtime_error("ComponentManager[]: Entity " +
                                 std::to_string(eId) +
                                 " Does not have component");
    }
    return it->second;
}

// ComponentService
std::unordered_map<std::type_index, ComponentManagerBasePtr>&
ComponentService::ComponentManagers() {
    static std::unordered_map<std::type_index, ComponentManagerBasePtr>
        MANAGERS;
    return MANAGERS;
}
}  // namespace Components
