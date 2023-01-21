#include "NameComponent.h"

#include <Messages/MessageBus.h>

// NameComponent
Components::NameComponent::NameComponent(const std::string& name)
    : mName(name) {}

const std::string& Components::NameComponent::name() const { return mName; }

std::ostream& operator<<(std::ostream& os,
                         const Components::NameComponent& rhs) {
    os << rhs.name() << std::endl;
    return os;
}

// NameComponentManager
Entities::UUID Components::NameComponentManager::getUUID(
    const std::string& name) {
    for (auto it = begin(); it != end(); ++it) {
        if (it->name() == name) {
            return it.id();
        }
    }
    return Messages::NO_TYPE;
}

// GetName
std::string Components::GetName(const Entities::UUID& eId) {
    using namespace Components;
    auto& cm = GameObjects::Get<NameComponentManager>();
    return cm.hasEntity(eId) ? cm[eId].name() : "???";
}

Entities::UUID Components::GetUUID(const std::string& name) {
    using namespace Components;
    return GameObjects::Get<NameComponentManager>().getUUID(name);
}
