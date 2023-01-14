#include "Messager.h"

namespace Messages {
// Messager
Messager::Messager() : mId(Entities::generateUUID()) {
    std::cerr << mId << std::endl;
}

Messager::operator Entities::UUID() const { return mId; }

Entities::UUID Messager::id() const { return mId; }

// Receiver
Receiver::~Receiver() {
    for (auto& handle : mSubscriptions) {
        GetMessageBus().unsubscribe(handle);
    }
}

void Receiver::attachSubscription(const MessageHandle& handle) {
    mSubscriptions.push_back(handle);
}
}  // namespace Messages
