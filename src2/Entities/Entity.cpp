#include "Entity.h"

namespace Entities {
// Entity
Entity::~Entity() {
    Messages::GetMessageBus().sendImmediateMessage(
        std::make_unique<EntityUnsubMessage>(id()));
}

const Messages::MessageT& Entity::TYPE() {
    static Messages::MessageT TYPE = typeid(Entity).name();
    return TYPE;
}

// EntityUnsubMessage
EntityUnsubMessage::EntityUnsubMessage(UUID eId)
    : Messages::Message(Entity::TYPE(), Entity::MessageCode::Unsub),
      mEId(eId) {}

UUID EntityUnsubMessage::getId() const { return mEId; }
}  // namespace Entities
