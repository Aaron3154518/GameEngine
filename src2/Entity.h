#ifndef ENTITY_H
#define ENTITY_H

#include <array>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "Component.h"
#include "ComponentManager.h"
#include "UUID.h"

class Entity {
   public:
    Entity() : mId(Game::generateUUID()) { std::cerr << mId << std::endl; }
    virtual ~Entity() = default;

   protected:
    template <class CompT, class... ArgTs>
    void addComponent(ArgTs&&... args) {
        ComponentService::getComponentManager<CompT>().newComponent(
            mId, std::forward<ArgTs>(args)...);
        std::cerr << "Added: " << typeid(CompT).name() << std::endl;
    }

   private:
    Game::UUID mId;
};

#endif
