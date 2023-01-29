#include "CommandService.h"

#include <Test.h>

REGISTER(Services::CommandService::Message, CommandMessage);

namespace Services {
// CommandService
void CommandService::service_init() {
    subscribeTo<Messages::Message<>>([](const auto& msg) {
        if (!msg.opts.quiet) {
            std::cerr << "\033[1;34m[Message]\033[0m "
                      << Messages::MessageTypes::GetName(msg.id()) << " "
                      << msg.code << std::endl;
        }
    });
}

bool CommandService::checkInput(std::queue<Messages::MessagePtr>& msgs,
                                CRITICAL_SECTION* msgQueue) {
    char* str = (char*)malloc(sizeof(char) * 255);
    std::cin.getline(str, 255);
    std::stringstream ss(str);
    free(str);

    std::string service;
    if (!(ss >> service)) {
        return true;
    }

    if (service == "quit") {
        EnterCriticalSection(msgQueue);
        msgs.push(std::make_unique<Message>(Quit));
        LeaveCriticalSection(msgQueue);
        return false;
    }

    int code;
    if (!(ss >> code)) {
        return true;
    }

    std::string line = ss.str();
    line = (long long unsigned int)ss.tellg() >= line.size()
               ? ""
               : line.substr(ss.tellg());

    auto msg = Messages::MessageTypes::Generate(service, code, line);
    if (!msg) {
        return true;
    }

    EnterCriticalSection(msgQueue);
    msgs.push(std::move(msg));
    LeaveCriticalSection(msgQueue);

    return true;
}
}  // namespace Services
