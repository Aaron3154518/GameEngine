#ifndef ENTITIY_CONTAINER_H
#define ENTITIY_CONTAINER_H

#include <Entities/Entity.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Messages/GameObjects.h>
#include <Messages/MessageBus.h>
#include <Utils/Templates.h>

#include <memory>
#include <vector>

namespace Entities {
class EntityContainerBase : public Entity {
   public:
    virtual ~EntityContainerBase() = default;

    bool empty() const;
    size_t size() const;

    void remove(const UUID& id);

   protected:
    std::vector<EntityPtr> mEntities;
};

template <class EntityT>
class EntityContainer : public EntityContainerBase {
    static_assert(std::is_base_of<Entity, EntityT>::value,
                  "EntityContainer<>: Containee must extend from Entity class");

   public:
    template <class... ArgTs>
    void add(ArgTs&&... args) {
        static_assert(std::is_constructible<EntityT, EntityContainerBase*,
                                            ArgTs...>::value,
                      "EntityContainer::add<>: Containee is not constructible "
                      "with the given arguments");
        mEntities.push_back(
            GameObjects::New<EntityT>(this, std::forward<ArgTs>(args)...));
    }

    EntityT& back() const { return static_cast<EntityT&>(*mEntities.back()); }

    template <class F, class... ArgTs>
    typename std::enable_if<std::is_member_function_pointer<F>::value>::type
    forEach(const F& func, ArgTs&&... args) {
        forEach(
            [func](EntityT& e, ArgTs&&... args) {
                (e.*func)(std::forward<ArgTs>(args)...);
            },
            std::forward<ArgTs>(args)...);
    }

    template <class F, class... ArgTs>
    typename std::enable_if<!std::is_member_function_pointer<F>::value>::type
    forEach(const F& func, ArgTs&&... args) {
        static_assert(std::is_invocable<F, EntityT&, ArgTs...>::value,
                      "ComponentManager::forEach(): Function must accept a "
                      "Component reference and the provided arguments");
        for (auto& ptr : mEntities) {
            func(static_cast<EntityT&>(*ptr), std::forward<ArgTs>(args)...);
        }
    }
};

class EntityContainee {
    friend class EntityContainerBase;

   public:
    EntityContainee(EntityContainerBase* container);

   protected:
    void removeFromContainer(const UUID& eId);

   private:
    EntityContainerBase* mContainer;
};
}  // namespace Entities

#endif