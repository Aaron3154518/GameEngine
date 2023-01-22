#include "MessageBus.h"

namespace Messages {
// MessageBus
void MessageBus::sendMessage(const Message<>& msg) {
    if (msg.src != NO_TYPE) {
        if (msg.code != NO_CODE) {
            // Subscribed to this message type and code
            sendMessage(msg, subscribers[msg.src][msg.code]);
        }
        // Subscribed to all messages from this type
        sendMessage(msg, subscribers[msg.src][NO_CODE]);
    }
    // Subscribed to all messages
    sendMessage(msg, subscribers[NO_TYPE][NO_CODE]);
}

void MessageBus::sendMessage(const Message<>& msg,
                             const std::vector<EntityCallback>& targets) {
    auto& target = msg.opts.target;
    if (target == NO_TYPE) {
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

MessageHandle MessageBus::subscribe(const MessageFunc& callback,
                                    Entities::UUID eId,
                                    const Entities::UUID& msgType,
                                    EnumT msgCode) {
    auto& cnt = entity_counts[eId];
    subscribers[msgType][msgCode].push_back({eId, cnt, callback});
    return MessageHandle{eId, cnt++, msgType, msgCode};
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
