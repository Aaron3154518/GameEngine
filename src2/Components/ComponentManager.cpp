#include "ComponentManager.h"

#include <Entities/Entity.h>

namespace Components {
// ComponentManagerBase
void ComponentManagerBase::init() {
    attachSubscription(Messages::GetMessageBus()
                           .subscribe<Entities::EntityUnsubService::Message>(
                               [this](const auto& m) {
                                   auto it = mComponents.find(m.data);
                                   if (it != mComponents.end()) {
                                       mComponents.erase(it);
                                   }
                               },
                               id(),
                               GameObjects::Get<Entities::EntityUnsubService>(),
                               Entities::EntityUnsubService::Unsub));

    manager_init();
}

void ComponentManagerBase::manager_init() {}

bool ComponentManagerBase::hasEntity(Entities::UUID eId) {
    return mComponents.find(eId) != mComponents.end();
}

size_t ComponentManagerBase::size() const { return mComponents.size(); }

Component& ComponentManagerBase::operator[](Entities::UUID eId) const {
    auto it = mComponents.find(eId);
    if (it == mComponents.end()) {
        throw std::runtime_error("ComponentManager[]: Entity " +
                                 std::to_string(eId) +
                                 " Does not have component");
    }
    return *it->second;
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
    return mIt->second.get();
}
Component& ComponentManagerBase::iterator_base::operator*() const {
    return *mIt->second;
}

// Prefix ++
ComponentManagerBase::iterator_base&
ComponentManagerBase::iterator_base::operator++() {
    ++mIt;
    return *this;
}

const Entities::UUID& ComponentManagerBase::iterator_base::id() const {
    return mIt->first;
}
}  // namespace Components
