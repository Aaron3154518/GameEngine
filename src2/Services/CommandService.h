#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include <Components/Component.h>
#include <Components/NameComponent.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>
#include <Windows.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Services {
class CommandComponent
    : public Components::DataComponent<std::vector<Messages::EnumT>> {
   public:
    using Components::DataComponent<
        std::vector<Messages::EnumT>>::DataComponent;
};
typedef Components::ComponentManager<CommandComponent> CommandComponentManager;

class CommandService : public Service {
   public:
    enum Code : Messages::EnumT { Command = 0 };
    struct CommandData {
        std::string line;
        Messages::EnumT cmdCode;
    };
    typedef Messages::Message<CommandService, CommandData> Message;

    bool checkInput(std::queue<Messages::MessagePtr>& msgs,
                    CRITICAL_SECTION* msgQueue);

   private:
    void service_init();
};
}  // namespace Services

#endif
