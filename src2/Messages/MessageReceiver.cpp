#include "MessageReceiver.h"

namespace Messages {
// MessageReceiver
MessageReceiver::MessageReceiver() : mId(Entities::generateUUID()) {
    std::cerr << mId << std::endl;
}

MessageReceiver::~MessageReceiver() {
    for (auto& handle : mSubscriptions) {
        GetMessageBus().unsubscribe(handle);
    }
}

MessageReceiver::operator Entities::UUID() const { return mId; }

Entities::UUID MessageReceiver::id() const { return mId; }

void MessageReceiver::attachSubscription(const MessageHandle& handle) {
    mSubscriptions.push_back(handle);
}
}  // namespace Messages
