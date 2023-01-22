#include "Test.h"

// MyService
void MyService::service_init() {
    setName("MyService");
    attachSubscription(Messages::GetMessageBus().subscribe<CountMessage>(
        [this](const auto& m) {
            mCnt += m.data;
            Messages::GetMessageBus().sendMessage(
                CountMessage(mCnt, MyService::Code::PrintCount));
        },
        id(), id(), MyService::Code::IncreaseCount));

    GameObjects::Get<Services::CommandComponentManager>().newComponent(
        id(), std::vector<Messages::EnumT>{MyService::Code::PrintCount,
                                           MyService::Code::IncreaseCount});
    GameObjects::Get<Services::CommandService>().subscribe(id());
    attachSubscription(
        Messages::GetMessageBus().subscribe<Services::CommandService::Message>(
            [this](const auto& m) { onCommandMessage(m); }, id(),
            GameObjects::Get<Services::CommandService>(),
            Services::CommandService::Command));
}

void MyService::onCommandMessage(const Services::CommandService::Message& m) {
    auto code = m.data.cmdCode;
    switch (code) {
        case MyService::Code::PrintCount:
        case MyService::Code::IncreaseCount: {
            int val = mCnt;
            if (code == MyService::Code::IncreaseCount) {
                std::stringstream ss(m.data.line);
                if (!(ss >> val)) {
                    val = 1;
                }
            }
            Messages::GetMessageBus().sendMessage(
                CountMessage(val, static_cast<MyService::Code>(code)));
            break;
        }
        default:
            break;
    }
}
