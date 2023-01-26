#include "Entity.h"

namespace Entities {
// Entity
Entity::~Entity() {
    Messages::GetMessageBus().sendMessage(
        EntityUnsubService::Message(id(), EntityUnsubService::Unsub));
}

// EntitiyUnsubService
void EntityUnsubService::service_init() {
    setName("EntityUnsubService");
    Messages::MessageTypes::Register<Message>("EntityUnsubService");
}
}  // namespace Entities
