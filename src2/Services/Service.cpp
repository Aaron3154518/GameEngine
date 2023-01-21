#include "Service.h"

#include <Entities/Entity.h>

namespace Services {
// Service
void Service::subscribe(const Entities::UUID& eId) {
    auto it = std::find(mEntities.begin(), mEntities.end(), eId);
    if (it == mEntities.end()) {
        mEntities.push_back(eId);
    }
}

void Service::unsubscribe(const Entities::UUID& eId) {
    auto it = std::find(mEntities.begin(), mEntities.end(), eId);
    if (it != mEntities.end()) {
        mEntities.erase(it);
    }
}

void Service::init() {
    attachSubscription(
        Messages::GetMessageBus().subscribe<Entities::EntityUnsubMessage>(
            [this](const Entities::EntityUnsubMessage& m) {
                unsubscribe(m.eId);
            },
            id(), GameObjects::Get<Entities::EntityUnsubService>(),
            Entities::EntityUnsubService::Unsub));

    service_init();
}

void Service::service_init() {}
}  // namespace Services
