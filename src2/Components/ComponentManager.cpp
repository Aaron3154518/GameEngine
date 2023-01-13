#include "ComponentManager.h"

#include <Entities/Entity.h>

namespace Components {
// ComponentManagerBase
ComponentManagerBase::ComponentManagerBase() {
    attachSubscription(
        Messages::GetMessageBus().subscribe<Entities::EntityUnsubMessage>(
            [this](const Entities::EntityUnsubMessage& m) {
                auto it = mComponents.find(m.getId());
                if (it != mComponents.end()) {
                    mComponents.erase(it);
                }
            },
            id(), Entities::Entity::TYPE(), Entities::Entity::Unsub));
}

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

// ComponentManagerBase::iterator_base
ComponentManagerBase::iterator_base::iterator_base(const map_iterator& it)
    : mIt(it) {}

bool ComponentManagerBase::iterator_base::operator==(
    const iterator_base& rhs) const {
    return mIt == rhs.mIt;
}
bool ComponentManagerBase::iterator_base::operator!=(
    const iterator_base& rhs) const {
    return !(*this == rhs);
}

Component* ComponentManagerBase::iterator_base::operator->() const {
    return &mIt->second;
}
Component& ComponentManagerBase::iterator_base::operator*() const {
    return mIt->second;
}

// Prefix ++
ComponentManagerBase::iterator_base&
ComponentManagerBase::iterator_base::operator++() {
    ++mIt;
    return *this;
}

// ComponentService
std::unordered_map<std::type_index, ComponentManagerBasePtr>&
ComponentService::ComponentManagers() {
    static std::unordered_map<std::type_index, ComponentManagerBasePtr>
        MANAGERS;
    return MANAGERS;
}
}  // namespace Components
