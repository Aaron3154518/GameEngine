#include "MessageBus.h"

namespace Messages {
// const MessageT& MessageBus::NO_TYPE() {
//   static const MessageT TYPE;
// return TYPE;
//}

// MessageBus
void MessageBus::queueMessage(MessagePtr msg) { messages.push(std::move(msg)); }

void MessageBus::sendImmediateMessage(MessagePtr msg) { sendMessage(msg); }

void MessageBus::sendMessages() {
    while (!messages.empty()) {
        sendMessage(messages.front());
        messages.pop();
    }
}

void MessageBus::sendMessage(const MessagePtr& msg) {
    if (msg->type() != NO_TYPE) {
        if (msg->code() != NO_CODE) {
            // Subscribed to this message type and code
            sendMessage(msg, subscribers[msg->type()][msg->code()]);
        }
        // Subscribed to all messages from this type
        sendMessage(msg, subscribers[msg->type()][NO_CODE]);
    }
    // Subscribed to all messages
    sendMessage(msg, subscribers[NO_TYPE][NO_CODE]);
}

void MessageBus::sendMessage(const MessagePtr& msg,
                             const std::vector<EntityCallback>& targets) {
    auto& target = msg->target();
    if (target == NO_TYPE) {
        for (auto& callback : targets) {
            callback.func(msg.get());
        }
    } else {
        for (auto& callback : targets) {
            if (callback.eId == target) {
                callback.func(msg.get());
            }
        }
    }
}

MessageHandle MessageBus::subscribe(
    const std::function<void(const Message&)>& callback, Entities::UUID eId,
    const MessageT& msgType, EnumT msgCode) {
    return subscribe([callback](const Message* m) { callback(*m); }, eId,
                     msgType, msgCode);
}

MessageHandle MessageBus::subscribe(const MessageFunc& callback,
                                    Entities::UUID eId, const MessageT& msgType,
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
