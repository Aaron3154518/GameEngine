#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <Components/Component.h>
#include <Entities/UUID.h>

#include <iostream>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace Components {
class ComponentManagerBase {
   public:
    virtual ~ComponentManagerBase() = default;
};

typedef std::unique_ptr<ComponentManagerBase> ComponentManagerBasePtr;

template <class CompT>
class ComponentManager : public ComponentManagerBase {
    static_assert(std::is_base_of<Component, CompT>::value,
                  "ComponentManager<>: Type must derive from Component");

   public:
    template <class... ArgTs>
    const CompT& newComponent(Entities::UUID eId, ArgTs&&... args) {
        static_assert(std::is_constructible<CompT, ArgTs...>::value,
                      "ComponentManager::newComponent(): Cannot construct "
                      "component with the given arguments");
        auto it = mComponents.find(eId);
        if (it == mComponents.end()) {
            it = mComponents.emplace(eId, CompT(std::forward<ArgTs>(args)...))
                     .first;
        } else {
            std::cerr << "Unsubscribe Me" << std::endl;
            it->second = CompT(std::forward<ArgTs>(args)...);
        }
        return it->second;
    }

    bool hasEntity(Entities::UUID eId) {
        return mComponents.find(eId) != mComponents.end();
    }

    const CompT& operator[](Entities::UUID eId) {
        auto it = mComponents.find(eId);
        if (it == mComponents.end()) {
            throw std::runtime_error("ComponentManager[]: Entity " +
                                     std::to_string(eId) + " Does not have " +
                                     typeid(CompT).name());
        }
        return it->second;
    }

   private:
    std::unordered_map<Entities::UUID, CompT> mComponents;
};

class ComponentService {
   public:
    template <class CompT>
    static ComponentManager<CompT>& getComponentManager() {
        auto& cm = ComponentManagers();
        auto key = std::type_index(typeid(CompT));
        auto it = cm.find(key);
        if (it == cm.end()) {
            it = cm.emplace(key, std::make_unique<ComponentManager<CompT>>())
                     .first;
        }
        auto ptr = static_cast<ComponentManager<CompT>*>(it->second.get());
        if (!ptr) {
            throw std::runtime_error(
                "ComponentService::getComponentManager(): Could not get "
                "manager for " +
                std::string(key.name()));
        }
        return *ptr;
    }

   private:
    static std::unordered_map<std::type_index, ComponentManagerBasePtr>&
    ComponentManagers();
};
}  // namespace Components

#endif
