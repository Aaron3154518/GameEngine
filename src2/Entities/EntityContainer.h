#ifndef ENTITIY_CONTAINER_H
#define ENTITIY_CONTAINER_H

#include <Entities/Entity.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Messages/GameObjects.h>
#include <Messages/MessageBus.h>

#include <memory>
#include <vector>

namespace Entities {
template <class EntityT>
class EntityContainer : public Entity {
   public:
    enum MessageCode : Messages::EnumT { Remove = 0 };
    class Message : public Messages::Message<MessageCode, Entities::UUID> {
        using Messages::Message<MessageCode, Entities::UUID>::Message;
    };

    typedef std::unique_ptr<EntityT> EntityPtr;

    virtual ~EntityContainer() = default;

    template <class... ArgTs>
    void add(ArgTs&&... args) {
        mEntities.push_back(
            GameObjects::New<EntityT>(std::forward<ArgTs>(args)...));
    }

    const EntityPtr& back() const { return mEntities.back(); }

    bool empty() const { return mEntities.empty(); }
    size_t size() const { return mEntities.size(); }

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
    void init() {
        Entity::subscribeTo<Message>(
            [this](const Message& m) {
                auto& id = m.data;
                auto it = std::find_if(
                    mEntities.begin(), mEntities.end(),
                    [id](const EntityPtr& eptr) { return eptr->id() == id; });
                if (it != mEntities.end()) {
                    EventSystem::runNextFrame(
                        [ptr = it->release()]() { delete ptr; });
                    mEntities.erase(it);
                }
            },
            Remove);
    }

    std::vector<EntityPtr> mEntities;
};
}  // namespace Entities

#endif