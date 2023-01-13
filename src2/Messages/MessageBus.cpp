#include "MessageBus.h"

namespace Messages {
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
    if (msg->type() != "") {
        if (msg->code() != -1) {
            // Subscribed to this message type and code
            for (auto& callback : subscribers[msg->type()][msg->code()]) {
                callback.func(*msg);
            }
        }
        // Subscribed to all messages from this type
        for (auto& callback : subscribers[msg->type()][-1]) {
            callback.func(*msg);
        }
    }
    // Subscribed to all messages
    for (auto& callback : subscribers[""][-1]) {
        callback.func(*msg);
    }
}

MessageHandle MessageBus::subscribe(Entities::UUID eId, const MessageT& msgType,
                                    EnumT msgCode,
                                    const MessageFunc& callback) {
    auto& cnt = entity_counts[eId];
    subscribers[msgType][msgCode].push_back({eId, cnt, callback});
    return MessageHandle{eId, cnt++, msgType, msgCode};
}

MessageHandle MessageBus::subscribe(Entities::UUID eId, const MessageT& msgType,
                                    const MessageFunc& callback) {
    return subscribe(eId, msgType, -1, callback);
}

MessageHandle MessageBus::subscribe(Entities::UUID eId,
                                    const MessageFunc& callback) {
    return subscribe(eId, "", callback);
}

void MessageBus::unsubscribe(MessageHandle handle) {
    auto& callbackList = subscribers[handle.type][handle.code];
    callbackList.erase(std::remove_if(callbackList.begin(), callbackList.end(),
                                      [handle](const EntityCallback& ecb) {
                                          return ecb.eId == handle.eId &&
                                                 ecb.eNum == handle.eNum;
                                      }));
}

MessageBus& GetMessageBus() {
    static MessageBus MSG_BUS;
    return MSG_BUS;
}
}  // namespace Messages
