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

    bool hasEntity(Entities::UUID eId);

    const Component& operator[](Entities::UUID eId) const;

   protected:
    std::unordered_map<Entities::UUID, Component> mComponents;
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
        return static_cast<const CompT&>(it->second);
    }

    const CompT& operator[](Entities::UUID eId) const {
        return static_cast<const CompT&>(ComponentManagerBase::operator[](eId));
    }

    class iterator {
        typedef std::unordered_map<Entities::UUID, Component>::iterator
            map_iterator;

       public:
        iterator(const map_iterator& it) : mIt(it) {}

        bool operator==(const iterator& rhs) const { return mIt == rhs.mIt; }
        bool operator!=(const iterator& rhs) const { return !(*this == rhs); }

        CompT* operator->() { return static_cast<CompT*>(&mIt->second); }
        CompT& operator*() { return static_cast<CompT&>(mIt->second); }

        // Prefix ++
        iterator& operator++() {
            ++mIt;
            return *this;
        }
        // Postfix ++
        iterator operator++(int) {
            iterator tmp(mIt);
            ++*this;
            return tmp;
        }

       private:
        map_iterator mIt;
    };

    iterator begin() { return iterator(mComponents.begin()); }
    iterator end() { return iterator(mComponents.end()); }
};

class ComponentService {
   public:
    template <class CompManT>
    static CompManT& Get() {
        auto& cm = ComponentManagers();
        auto key = std::type_index(typeid(CompManT));
        auto it = cm.find(key);
        if (it == cm.end()) {
            it = cm.emplace(key, std::make_unique<CompManT>()).first;
        }
        auto ptr = static_cast<CompManT*>(it->second.get());
        if (!ptr) {
            throw std::runtime_error(
                "ComponentService::Get(): Could not get "
                "manager for " +
                std::string(key.name()));
        }
        return *ptr;
    }

    template <class CompT, class CompManT = ComponentManager<CompT>>
    static CompT& Get(Entities::UUID eId) {
        static_assert(std::is_base_of<Component, CompT>::value,
                      "ComponentService::Get(): Component type must derive "
                      "from Component");
        static_assert(
            std::is_base_of<ComponentManagerBase, CompManT>::value,
            "ComponentService::Get(): ComponentManager type must derive "
            "from ComponentManagerBase");
        CompManT& cm = Get<CompManT>();
        return cm[eId];
    }

   private:
    static std::unordered_map<std::type_index, ComponentManagerBasePtr>&
    ComponentManagers();
};
}  // namespace Components

#endif
