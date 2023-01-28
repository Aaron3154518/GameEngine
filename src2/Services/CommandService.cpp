#include "CommandService.h"

#include <Test.h>

REGISTER(Services::CommandService::Message, CommandMessage);

namespace Services {
// CommandService
void CommandService::service_init() {
    subscribeTo<Messages::Message<>>([](const auto& msg) {
        std::cerr << "\033[1;34m[Message]\033[0m " << GetName(msg.id()) << " "
                  << msg.code << std::endl;
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

    auto msg = Generate(service, code, line);
    if (!msg) {
        return true;
    }

    EnterCriticalSection(msgQueue);
    msgs.push(std::move(msg));
    LeaveCriticalSection(msgQueue);

    return true;
}

// Static methods
Messages::MessagePtr CommandService::Generate(const std::string& name,
                                              Messages::EnumT code,
                                              const std::string& line) {
    auto it = GenMap().find(name);
    if (it == GenMap().end()) {
        return nullptr;
    }
    return it->second(code, line);
}

std::string CommandService::GetName(const std::type_index& id) {
    auto it = NameMap().find(id);
    return it == NameMap().end() ? id.name() : it->second;
}

std::unordered_map<std::string, CommandService::MsgGenFunc<>>&
CommandService::GenMap() {
    static std::unordered_map<std::string, MsgGenFunc<>> GEN_MAP;
    return GEN_MAP;
}

std::unordered_map<std::type_index, std::string>& CommandService::NameMap() {
    static std::unordered_map<std::type_index, std::string> NAME_MAP;
    return NAME_MAP;
}
}  // namespace Services
