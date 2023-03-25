#include "EntityContainer.h"

namespace Entities {
// EntityContainerBase
bool EntityContainerBase::empty() const { return mEntities.empty(); }
size_t EntityContainerBase::size() const { return mEntities.size(); }

void EntityContainerBase::remove(const UUID& id) {
    auto it =
        std::find_if(mEntities.begin(), mEntities.end(),
                     [id](const EntityPtr& eptr) { return eptr->id() == id; });
    if (it != mEntities.end()) {
        EventSystem::runNextFrame([ptr = it->release()]() { delete ptr; });
        mEntities.erase(it);
    }
}

// EntityContainee
EntityContainee::EntityContainee(EntityContainerBase* container)
    : mContainer(container) {}

void EntityContainee::removeFromContainer(const UUID& eId) {
    if (!mContainer) {
        // std::cerr << "EntityContainee::removeFromContainer(): Container is
        // null"
        //<< std::endl;
        return;
    }
    mContainer->remove(eId);
    mContainer = nullptr;
}
}  // namespace Entities