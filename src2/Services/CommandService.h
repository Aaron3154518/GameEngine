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
    struct CommandData {
        std::string line;
        Messages::EnumT cmdCode;
    };

    enum Msg_Code : Messages::EnumT { Quit = 0 };
    enum Cmd_Code : Messages::EnumT { Command = 0 };
    typedef Messages::Message<CommandService, Msg_Code> Message;
    typedef Messages::Message<CommandService, Cmd_Code, CommandData>
        CommandMessage;

    bool checkInput(std::queue<Messages::MessagePtr>& msgs,
                    CRITICAL_SECTION* msgQueue);

   private:
    void service_init();
};
}  // namespace Services

#endif
