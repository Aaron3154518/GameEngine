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
class Entity : public Messages::Receiver {
   public:
    virtual ~Entity();

   protected:
    template <class CompManT, class... ArgTs>
    void addComponent(ArgTs&&... args) {
        Components::ComponentService::Get<CompManT>().newComponent(
            id(), std::forward<ArgTs>(args)...);
        std::cerr << "Added Component to: " << typeid(CompManT).name()
                  << std::endl;
    }
};

class EntityUnsubMessage : public Messages::Message {
   public:
    enum Code : Messages::EnumT { Unsub = 0 };

    EntityUnsubMessage(UUID eId);

    UUID getId() const;

   private:
    const UUID mEId;
};

class EntityUnsubService : public Services::Service<EntityUnsubMessage::Code> {
   public:
    static const EntityUnsubService& EUS();
};
}  // namespace Entities

#endif
