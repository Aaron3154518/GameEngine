#ifndef ENTITY_H
#define ENTITY_H

#include <Components/Component.h>
#include <Components/ComponentManager.h>
#include <Entities/UUID.h>
#include <Messages/MessageBus.h>
#include <Messages/MessageReceiver.h>

#include <array>
#include <iostream>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace Entities {
class Entity : public Messages::MessageReceiver {
   public:
    virtual ~Entity();

    static const Messages::MessageT& TYPE();
    enum MessageCode : Messages::EnumT { Unsub = 0 };

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
    EntityUnsubMessage(UUID eId);

    UUID getId() const;

   private:
    const UUID mEId;
};
}  // namespace Entities

#endif
