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

    operator UUID() const;

   protected:
    template <class CompManT, class... ArgTs>
    void addComponent(ArgTs&&... args) {
        Components::ComponentService::Get<CompManT>().newComponent(
            mId, std::forward<ArgTs>(args)...);
        std::cerr << "Added Component to: " << typeid(CompManT).name()
                  << std::endl;
    }

   private:
    UUID mId;
};
}  // namespace Entities

#endif
