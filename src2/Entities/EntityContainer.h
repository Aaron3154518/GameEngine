#ifndef ENTITIY_CONTAINER_H
#define ENTITIY_CONTAINER_H

#include <Entities/Entity.h>
#include <Messages/GameObjects.h>
#include <Messages/MessageBus.h>

#include <memory>
#include <vector>

namespace Entities {
template <class EntityT>
class EntityContainer : public Entity {
   public:
    typedef std::unique_ptr<EntityT> EntityPtr;

    virtual ~EntityContainer() = default;

    void add() { mEntities.push_back(GameObjects::New<EntityT>()); }

    const EntityPtr& back() const { return mEntities.back(); }

    bool empty() const { return mEntities.empty(); }
    size_t size() const { return mEntities.size(); }

    virtual bool remove(const EntityT& e) { return false; }

    void prune() {
        auto it = mEntities.begin();
        while (it != mEntities.end()) {
            if (remove(**it)) {
                it = mEntities.erase(it);
            } else {
                ++it;
            }
        }
    }

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
        prune();
        for (auto& ptr : mEntities) {
            func(*ptr, std::forward<ArgTs>(args)...);
        }
    }

    template <class MsgT, class F>
    void subscribeTo(const F& callback, typename MsgT::Code code) {
        Entity::subscribeTo<MsgT>(
            [this, callback](const MsgT& m) { forEach(callback, m); }, code);
    }

    template <class MsgT, class F>
    void subscribeTo(const F& callback) {
        Entity::subscribeTo<MsgT>(
            [this, callback](const MsgT& m) { forEach(callback, m); });
    }

   private:
    std::vector<EntityPtr> mEntities;
};
}  // namespace Entities

#endif