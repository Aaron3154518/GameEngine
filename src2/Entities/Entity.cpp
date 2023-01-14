#include "Entity.h"

namespace Entities {
// Entity
Entity::~Entity() {
    Messages::GetMessageBus().sendImmediateMessage(
        std::make_unique<EntityUnsubMessage>(id()));
}

// EntityUnsubMessage
EntityUnsubMessage::EntityUnsubMessage(UUID eId)
    : Messages::Message(GameObjects::Get<EntityUnsubService>(),
                        EntityUnsubMessage::Unsub),
      mEId(eId) {}

UUID EntityUnsubMessage::getId() const { return mEId; }

// EntitiyUnsubService
void EntityUnsubService::init() { setName("EntityUnsubService"); }
}  // namespace Entities
