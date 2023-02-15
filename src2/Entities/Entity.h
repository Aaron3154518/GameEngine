#ifndef ENTITY_H
#define ENTITY_H

#include <Components/Component.h>
#include <Components/ComponentManager.h>
#include <Entities/UUID.h>
#include <Messages/MessageBus.h>
#include <Messages/Messager.h>
#include <Services/Service.h>

#include <array>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace Entities {
class Entity : public Messages::Messager {
   public:
    virtual ~Entity();

   protected:
    template <class CompManT, class... ArgTs>
    void addComponent(ArgTs&&... args) {
        GameObjects::Get<CompManT>().newComponent(id(),
                                                  std::forward<ArgTs>(args)...);
    }

    template <class CompManT>
    typename CompManT::Component& getComponent() {
        return Components::Get<CompManT>(id());
    }
};

class EntityUnsubService : public Services::Service {
   public:
    DATA_MESSAGE(Message, UUID, Unsub);
};
}  // namespace Entities

#endif
