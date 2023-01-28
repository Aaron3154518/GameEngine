#include "Messager.h"

#include <Messages/MessageBus.h>

namespace Messages {
// Messager
Messager::Messager() : mId(Entities::generateUUID()) {}
Messager::~Messager() {
    for (auto& handle : mSubscriptions) {
        GetMessageBus().unsubscribe(handle);
    }
}

Messager::operator Entities::UUID() const { return mId; }

Entities::UUID Messager::id() const { return mId; }

void Messager::init() {}
}  // namespace Messages
