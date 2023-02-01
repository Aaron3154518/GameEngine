#include "Test.h"

Entities::UUID P = Entities::generateUUID();
Entities::UUID E = Entities::generateUUID();
bool _ = CollisionService::NewType(P, E);

int MyService::count() const { return mCnt; }

// MyService
void MyService::service_init() {
    subscribeTo<CountMessage>([this](const CountMessage& m) { mCnt += m.data; },
                              IncreaseCount);
}

REGISTER(MyService::Message, MyMessage);

REGISTER(MyService::CountMessage, MyCountMessage, {
    std::stringstream ss(line);
    int cnt;
    return std::make_unique<Msg>(code,
                                 code == MyService::PrintCount
                                     ? GameObjects::Get<MyService>().count()
                                 : !(ss >> cnt) ? 1
                                                : cnt);
});