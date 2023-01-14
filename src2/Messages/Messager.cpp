#include "Messager.h"

#include <Components/NameComponent.h>

namespace Messages {
// Messager
Messager::Messager() : mId(Entities::generateUUID()) {}
Messager::~Messager() {
    for (auto& handle : mSubscriptions) {
        GetMessageBus().unsubscribe(handle);
    }
}

Messager::operator Entities::UUID() const { return mId; }

Entities::UUID Messager::id() const { return mId; }

void Messager::init() {}

void Messager::setName(const std::string& name) const {
    GameObjects::Get<Components::NameComponentManager>().newComponent(id(),
                                                                      name);
}

void Messager::attachSubscription(const MessageHandle& handle) {
    mSubscriptions.push_back(handle);
}
}  // namespace Messages
