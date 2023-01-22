#include "CommandService.h"

#include <Test.h>

namespace Services {
// CommandService
void CommandService::service_init() {
    attachSubscription(Messages::GetMessageBus().subscribe(
        [](const Messages::Message<>& msg) {
            std::cerr << "\033[1;34m[Message]\033[0m "
                      << Components::GetName(msg.src) << " " << msg.code
                      << std::endl;
        },
        id()));
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
        msgs.push(Message::New(Code::Quit));
        LeaveCriticalSection(msgQueue);
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

    auto& commMan = GameObjects::Get<CommandComponentManager>();
    if (std::find(mEntities.begin(), mEntities.end(), uuid) !=
            mEntities.end() &&
        commMan.hasEntity(uuid)) {
        auto& codes = commMan[uuid].get();
        if (std::find(codes.begin(), codes.end(), code) != codes.end()) {
            std::string msgStr = ss.str();
            msgStr = (long long unsigned int)ss.tellg() >= msgStr.size()
                         ? ""
                         : msgStr.substr(ss.tellg());
            EnterCriticalSection(msgQueue);
            msgs.push(
                CommandMessage::New({msgStr, code}, Code::Command, {uuid}));
            LeaveCriticalSection(msgQueue);
            return true;
        }
    }

    EnterCriticalSection(msgQueue);
    msgs.push(std::make_unique<Messages::Message<>>(uuid, code));
    LeaveCriticalSection(msgQueue);

    return true;
}
}  // namespace Services
