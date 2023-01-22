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
        std::cerr << "Added Component to: " << typeid(CompManT).name()
                  << std::endl;
    }
};

class EntityUnsubService : public Services::Service {
   public:
    enum Code : Messages::EnumT { Unsub = 0 };
    typedef Messages::Message<EntityUnsubService, UUID> Message;

   private:
    void service_init();
};
}  // namespace Entities

#endif
