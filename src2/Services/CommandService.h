#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include <Components/NameComponent.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace Services {
class CommandService : public Service {
   public:
    enum Code : Messages::EnumT { Command = 0 };

    bool checkInput();

   private:
    void init();
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
