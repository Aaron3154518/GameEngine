#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <Components/Component.h>
#include <Entities/UUID.h>
#include <Messages/GameObjects.h>
#include <Messages/Messager.h>

#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace Components {
class ComponentManagerBase : public Messages::Messager {
    friend class ComponentService;

   public:
    virtual ~ComponentManagerBase() = default;

    bool hasEntity(Entities::UUID eId);

    Component& operator[](Entities::UUID eId) const;

    size_t size() const;

   protected:
    std::unordered_map<Entities::UUID, ComponentPtr> mComponents;

   public:
    class iterator_base {
        typedef std::unordered_map<Entities::UUID, ComponentPtr>::iterator
            map_iterator;

       public:
        iterator_base(const map_iterator& it);
        virtual ~iterator_base() = default;

        bool operator==(const iterator_base& rhs) const;
        bool operator!=(const iterator_base& rhs) const;

        Component* operator->() const;
        Component& operator*() const;

        // Prefix ++
        iterator_base& operator++();

        const Entities::UUID& id() const;

       private:
        map_iterator mIt;
    };

   private:
    void init() final;
    virtual void manager_init();

   protected:
    template <class CompT>
    class iterator : public iterator_base {
       public:
        using iterator_base::iterator_base;

        CompT* operator->() {
            return static_cast<CompT*>(iterator_base::operator->());
        }
        CompT& operator*() {
            return static_cast<CompT&>(iterator_base::operator*());
        }
    };
};

typedef std::unique_ptr<ComponentManagerBase> ComponentManagerBasePtr;

template <class CompT>
class ComponentManager : public ComponentManagerBase {
    static_assert(std::is_base_of<Component, CompT>::value,
                  "ComponentManager<>: Type must derive from Component");

   public:
    typedef CompT Component;

    virtual ~ComponentManager() = default;

    template <class... ArgTs>
    CompT& newComponent(Entities::UUID eId, ArgTs&&... args) {
        static_assert(std::is_constructible<CompT, ArgTs...>::value,
                      "ComponentManager::newComponent(): Cannot construct "
                      "component with the given arguments");
        auto& comp = mComponents[eId];
        comp = std::make_unique<CompT>(std::forward<ArgTs>(args)...);
        return *static_cast<CompT*>(comp.get());
    }

    CompT& operator[](Entities::UUID eId) {
        return static_cast<CompT&>(ComponentManagerBase::operator[](eId));
    }

    template <typename F, class... ArgTs>
    typename std::enable_if<std::is_member_function_pointer<F>::value>::type
    forEach(const F& func, ArgTs&&... args) {
        forEach(
            [func](CompT& c, ArgTs&&... args) {
                (c.*func)(std::forward<ArgTs>(args)...);
            },
            std::forward<ArgTs>(args)...);
    }

    template <typename F, class... ArgTs>
    typename std::enable_if<!std::is_member_function_pointer<F>::value>::type
    forEach(const F& func, ArgTs&&... args) {
        static_assert(std::is_invocable<F, CompT&, ArgTs...>::value,
                      "ComponentManager::forEach(): Function must accept a "
                      "Component reference and the provided arguments");
        for (auto& comp : *this) {
            func(comp, std::forward<ArgTs>(args)...);
        }
    }

    typedef iterator<CompT> iterator;

    iterator begin() { return iterator(mComponents.begin()); }
    iterator end() { return iterator(mComponents.end()); }
};

template <class CompManT>
typename CompManT::Component& Get(Entities::UUID eId) {
    static_assert(std::is_base_of<ComponentManagerBase, CompManT>::value,
                  "Components::Get(): ComponentManager type must derive "
                  "from ComponentManagerBase");
    return GameObjects::Get<CompManT>()[eId];
}

class EntityComponents {
   public:
    EntityComponents(const Entities::UUID& id);

    const Entities::UUID& id() const { return mId; }

    template <class CompManT>
    bool has() const {
        return GameObjects::Get<CompManT>().hasEntity(mId);
    }

    template <class CompManT>
    typename CompManT::Component& get() const {
        return Components::Get<CompManT>(mId);
    }

    template <class CompManT>
    typename CompManT::Component::Data& getData() const {
        return get<CompManT>().get();
    }

    template <class CompManT>
    typename CompManT::Component::Data* getDataOpt() const {
        if (has<CompManT>()) {
            return &getData<CompManT>();
        }
        return nullptr;
    }

   private:
    Entities::UUID mId;
};
}  // namespace Components

#endif
