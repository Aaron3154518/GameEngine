#include "Entity.h"

namespace Entities {
// Entity
Entity::Entity() : mId(generateUUID()) { std::cerr << mId << std::endl; }

Entity::operator UUID() const { return mId; }
}  // namespace Entities
