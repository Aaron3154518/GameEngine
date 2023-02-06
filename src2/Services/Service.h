#ifndef SERVICE_H
#define SERVICE_H

#include <Components/ComponentManager.h>

namespace Services {
template <class CompT = Components::Component>
using Service = Components::ComponentManager<CompT>;
}  // namespace Services

#endif
