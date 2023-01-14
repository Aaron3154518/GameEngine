#include "Entity.h"

namespace Entities {
// Entity
Entity::~Entity() {
    Messages::GetMessageBus().sendImmediateMessage(
        std::make_unique<EntityUnsubMessage>(id()));
}

// EntityUnsubMessage
EntityUnsubMessage::EntityUnsubMessage(UUID eId)
    : Messages::Message(EntityUnsubService::EUS(), EntityUnsubMessage::Unsub),
      mEId(eId) {}

UUID EntityUnsubMessage::getId() const { return mEId; }

const EntityUnsubService& EntityUnsubService::EUS() {
    static EntityUnsubService eus;
    return eus;
}
}  // namespace Entities
