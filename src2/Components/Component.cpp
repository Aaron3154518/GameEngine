#include "Component.h"

namespace Components {
// Component
void Component::setActive(bool active) { mActive = active; }

bool Component::isActive() const { return mActive; }
}  // namespace Components
