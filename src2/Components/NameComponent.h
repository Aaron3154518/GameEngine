#ifndef NAME_COMPONENT_H
#define NAME_COMPONENT_H

#include <Components/ComponentManager.h>
#include <Entities/UUID.h>

#include <ostream>
#include <string>

namespace Components {
class NameComponent : public Component {
   public:
    NameComponent(const std::string& name);

    const std::string& name() const;

   private:
    const std::string mName;
};

class NameComponentManager : public ComponentManager<NameComponent> {
   public:
    Entities::UUID getUUID(const std::string& name);
};

std::string GetName(const Entities::UUID& eId);
Entities::UUID GetUUID(const std::string& name);
}  // namespace Components

std::ostream& operator<<(std::ostream& os,
                         const Components::NameComponent& rhs);

#endif