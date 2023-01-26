#include "MessageBus.h"

namespace Messages {
// MessageBus
void MessageBus::sendMessage(const Message<>& msg) {
    if (msg.id() != Entities::NullId()) {
        if (msg.code != NO_CODE) {
            // Subscribed to this sender and code
            sendMessage(msg, subscribers[msg.id()][msg.code]);
        }
        // Subscribed to this sender
        sendMessage(msg, subscribers[msg.id()][NO_CODE]);
    }
    // Subscribed to all messages
    sendMessage(msg, subscribers[Entities::NullId()][NO_CODE]);
}

void MessageBus::sendMessage(const Message<>& msg,
                             const std::vector<EntityCallback>& targets) {
    auto& target = msg.opts.target;
    if (target == GameObjects::Get<GameObjects::Void>().id()) {
        for (auto& callback : targets) {
            callback.func(msg);
        }
    } else {
        for (auto& callback : targets) {
            if (callback.eId == target) {
                callback.func(msg);
            }
        }
    }
}

MessageHandle MessageBus::subscribe(
    const std::function<void(const Message<>&)>& callback,
    const Entities::UUID& eId, const Entities::UUID& mId, EnumT msgCode) {
    auto& cnt = entity_counts[eId];
    subscribers[mId][msgCode].push_back({eId, cnt, callback});
    return MessageHandle{eId, cnt++, mId, msgCode};
}

void MessageBus::unsubscribe(MessageHandle handle) {
    auto& callbackList = subscribers[handle.type][handle.code];
    callbackList.erase(std::remove_if(callbackList.begin(), callbackList.end(),
                                      [handle](const EntityCallback& ecb) {
                                          return ecb.eId == handle.eId &&
                                                 ecb.eNum == handle.eNum;
                                      }),
                       callbackList.end());
}

MessageBus& GetMessageBus() {
    static MessageBus MSG_BUS;
    return MSG_BUS;
}
}  // namespace Messages
