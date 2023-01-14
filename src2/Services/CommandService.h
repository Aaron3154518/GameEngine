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
class CommandMessage : public Messages::Message {
   public:
    enum : Messages::EnumT { Command = 0 };

    CommandMessage(const std::string& line, Messages::EnumT code);

    const std::string& line() const;
    Messages::EnumT cmdCode() const;

   private:
    std::string mLine;
    Messages::EnumT mCmdCode;
};

class CommandService : public Service {
   public:
    bool checkInput();

   private:
    void init();
};
}  // namespace Services

#endif
