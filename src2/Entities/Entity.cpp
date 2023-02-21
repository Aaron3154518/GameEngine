#include "Entity.h"

REGISTER(Entities::EntityUnsubService::Message, EntityUnsubMessage,
         { return nullptr; });

namespace Entities {
// Entity
Entity::~Entity() {
    Messages::GetMessageBus().sendMessage(
        EntityUnsubService::Message(EntityUnsubService::Unsub, id()));
}
}  // namespace Entities
