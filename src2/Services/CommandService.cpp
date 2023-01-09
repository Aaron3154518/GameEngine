#include "CommandService.h"

namespace Services {
// CommandService
CommandService::CommandService() {
    Messages::MessageBus::subscribe([](const Messages::Message& msg) {
        std::cerr << "\033[1;34m[Message]\033[0m " << msg.type() << " "
                  << msg.code() << std::endl;
    });
}

bool CommandService::checkInput() {
    char* str = (char*)malloc(sizeof(char) * 255);
    std::cin.getline(str, 255);
    std::stringstream ss(str);
    free(str);

    std::string service;
    ss >> service;

    if (service == "quit") {
        return false;
    }

    if (service == "MyService") {
        int code;
        if (ss >> code) {
            Messages::MessageBus::queueMessage(
                Messages::Message(service, code));
        }
    }

    return true;
}
}  // namespace Services
