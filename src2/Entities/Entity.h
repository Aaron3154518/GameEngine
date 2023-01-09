#ifndef ENTITY_H
#define ENTITY_H

#include <Components/Component.h>
#include <Components/ComponentManager.h>
#include <Entities/UUID.h>

#include <array>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace Entities {
class Entity {
   public:
    Entity();
    virtual ~Entity() = default;

   protected:
    template <class CompT, class... ArgTs>
    void addComponent(ArgTs&&... args) {
        Components::ComponentService::getComponentManager<CompT>().newComponent(
            mId, std::forward<ArgTs>(args)...);
        std::cerr << "Added: " << typeid(CompT).name() << std::endl;
    }

   private:
    UUID mId;
};
}  // namespace Entities

#endif
