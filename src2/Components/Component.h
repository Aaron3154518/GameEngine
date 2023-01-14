#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>

namespace Components {
class Component {
   public:
    virtual ~Component() = default;
};

typedef std::unique_ptr<Component> ComponentPtr;
}  // namespace Components

#endif