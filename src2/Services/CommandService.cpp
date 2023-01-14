#include "CommandService.h"

#include <Test.h>

namespace Services {
// CommandMessage
CommandMessage::CommandMessage(const std::string& line, Messages::EnumT code)
    : Message(GameObjects::Get<CommandService>(), Command),
      mLine(line),
      mCmdCode(code) {}

const std::string& CommandMessage::line() const { return mLine; }

Messages::EnumT CommandMessage::cmdCode() const { return mCmdCode; }

// CommandService
void CommandService::init() {
    attachSubscription(Messages::GetMessageBus().subscribe(
        [](const Messages::Message& msg) {
            std::cerr << "\033[1;34m[Message]\033[0m "
                      << Components::GetName(msg.type()) << " " << msg.code()
                      << std::endl;
        },
        id()));
}

bool CommandService::checkInput() {
    char* str = (char*)malloc(sizeof(char) * 255);
    std::cin.getline(str, 255);
    std::stringstream ss(str);
    free(str);

    std::string service;
    if (!(ss >> service)) {
        return true;
    }

    if (service == "quit") {
        return false;
    }

    auto uuid = Components::GetUUID(service);
    if (uuid == Messages::NO_TYPE) {
        return true;
    }
    int code;
    if (!(ss >> code)) {
        return true;
    }

    std::string msgStr = ss.str();
    msgStr = (long long unsigned int)ss.tellg() >= msgStr.size()
                 ? ""
                 : msgStr.substr(ss.tellg());
    auto msg = std::make_unique<CommandMessage>(msgStr, code);
    msg->setTarget(uuid);
    Messages::GetMessageBus().queueMessage(std::move(msg));

    return true;
}
}  // namespace Services
