#include "CommandService.h"

#include <Test.h>

namespace Services {
// CommandService
void CommandService::service_init() {
    attachSubscription(Messages::GetMessageBus().subscribe(
        [](const Messages::BaseMessage& msg) {
            std::cerr << "\033[1;34m[Message]\033[0m "
                      << Components::GetName(msg.data.type) << " "
                      << msg.data.code << std::endl;
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
    Messages::GetMessageBus().queueMessage<CommandMessage>(
        Code::Command, {uuid}, msgStr, code);

    return true;
}

// CommandMessage
CommandMessage::CommandMessage(const MessageData& msg, const std::string& str,
                               Messages::EnumT code)
    : CommandMessageBase(msg), line(str), cmdCode(code) {}

}  // namespace Services
