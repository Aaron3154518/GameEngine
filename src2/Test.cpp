#include "Test.h"

// MyService
void MyService::service_init() {
    setName("MyService");
    attachSubscription(Messages::GetMessageBus().subscribe<MyCountMessage>(
        [this](const MyCountMessage& m) {
            mCnt += m.count;
            Messages::GetMessageBus().queueMessage<MyCountMessage>(
                MyServiceMessage::PrintCount, {}, mCnt);
        },
        id(), id(), MyServiceMessage::IncreaseCount));

    GameObjects::Get<Services::CommandComponentManager>().newComponent(
        id(), std::vector<Messages::EnumT>{MyServiceMessage::PrintCount,
                                           MyServiceMessage::IncreaseCount});
    GameObjects::Get<Services::CommandService>().subscribe(id());
    attachSubscription(
        Messages::GetMessageBus().subscribe<Services::CommandMessage>(
            [this](const Services::CommandMessage& m) { onCommandMessage(m); },
            id(), GameObjects::Get<Services::CommandService>(),
            Services::CommandService::Command));
}

void MyService::onCommandMessage(const Services::CommandMessage& m) {
    auto code = m.cmdCode;
    switch (code) {
        case MyServiceMessage::PrintCount:
        case MyServiceMessage::IncreaseCount: {
            int val = mCnt;
            if (code == MyServiceMessage::IncreaseCount) {
                std::stringstream ss(m.line);
                if (!(ss >> val)) {
                    val = 1;
                }
            }
            Messages::GetMessageBus().queueMessage<MyCountMessage>(
                static_cast<MyServiceMessage>(code), {}, val);
            break;
        }
        default:
            break;
    }
}
