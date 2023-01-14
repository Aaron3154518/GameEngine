#include "CommandService.h"

#include <Test.h>

namespace Services {
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
    ss >> service;

    if (service == "quit") {
        return false;
    }

    Messages::MessageT type = Components::GetUUID(service);

    if (type != Messages::NO_TYPE) {
        int code;
        if (ss >> code) {
            switch (code) {
                case MyServiceMessage::Hello:
                case MyServiceMessage::World:
                    Messages::GetMessageBus().queueMessage(
                        std::make_unique<Messages::Message>(type, code));
                    break;
                case MyServiceMessage::PrintCount:
                case MyServiceMessage::IncreaseCount: {
                    int val = 0;
                    if (code == MyServiceMessage::IncreaseCount) {
                        if (!(ss >> val)) {
                            val = 1;
                        }
                    }
                    auto m = std::make_unique<MyMessage>(
                        type, MyServiceMessage::IncreaseCount);
                    m->setCount(val);
                    Messages::GetMessageBus().queueMessage(std::move(m));
                } break;
                default:
                    break;
            }
        }
    }

    return true;
}
}  // namespace Services
