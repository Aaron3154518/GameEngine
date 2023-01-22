#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include <Components/Component.h>
#include <Components/NameComponent.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>

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

    bool checkInput(std::queue<Messages::MessagePtr>& msgs);

   private:
    void service_init();
};

typedef Messages::Message<CommandService, CommandService::Code>
    CommandMessageBase;
struct CommandMessage : public CommandMessageBase {
    CommandMessage(const MessageData& msg, const std::string& str,
                   Messages::EnumT code);

    std::string line;
    Messages::EnumT cmdCode;
};
}  // namespace Services

#endif
