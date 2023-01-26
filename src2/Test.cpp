#include "Test.h"

// MyService
void MyService::service_init() {
    setName("MyService");
    Messages::MessageTypes::Register<Message>("MyMessage");
    Messages::MessageTypes::Register<CountMessage>(
        "MyCountMessage",
        [this](Messages::EnumT code, const std::string& line) {
            auto msgCode = static_cast<CountMessageCode>(code);
            int val = mCnt;
            if (msgCode == IncreaseCount) {
                std::stringstream ss(line);
                if (!(ss >> val)) {
                    val = 1;
                }
            }
            return CountMessage::New(val, msgCode);
        });
    subscribeTo<CountMessage>([this](const auto& m) { mCnt += m.data; },
                              IncreaseCount);
}
