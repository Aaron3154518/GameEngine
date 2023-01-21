#include "Entity.h"

namespace Entities {
// Entity
Entity::~Entity() {
    Messages::GetMessageBus().sendImmediateMessage(
        Messages::make_unique_msg<EntityUnsubMessage>(EntityUnsubService::Unsub,
                                                      {}, id()));
}

// EntityUnsubMessage
EntityUnsubMessage::EntityUnsubMessage(const MessageData& msg, UUID eid)
    : EntityUnsubMessageBase(msg), eId(eid) {}

// EntitiyUnsubService
void EntityUnsubService::init() { setName("EntityUnsubService"); }
}  // namespace Entities
