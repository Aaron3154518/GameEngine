#include "MessageBus.h"

namespace Messages {
// MessageBus::MessageSubscribers
void MessageBus::MessageSubscribers::sendMessage(const Message& msg) {
    for (auto& func : mAllSubscribers) {
        func(msg);
    }
    for (auto& func : mSubscribers[msg.code()]) {
        func(msg);
    }
}

void MessageBus::MessageSubscribers::subscribe(EnumT code,
                                               const MessageFunc& callback) {
    mSubscribers[code].push_back(callback);
}
void MessageBus::MessageSubscribers::subscribe(const MessageFunc& callback) {
    mAllSubscribers.push_back(callback);
}

// MessageBus
void MessageBus::queueMessage(const Message& msg) { messages().push(msg); }

void MessageBus::sendMessages() {
    auto& msgs = messages();
    while (!msgs.empty()) {
        auto& msg = msgs.front();
        for (auto& func : allSubscribers()) {
            func(msg);
        }
        subscribers()[msg.type()].sendMessage(msg);
        msgs.pop();
    }
}

void MessageBus::subscribe(const MessageT& msgType, EnumT msgCode,
                           const MessageFunc& callback) {
    subscribers()[msgType].subscribe(msgCode, callback);
}

void MessageBus::subscribe(const MessageT& msgType,
                           const MessageFunc& callback) {
    subscribers()[msgType].subscribe(callback);
}

void MessageBus::subscribe(const MessageFunc& callback) {
    allSubscribers().push_back(callback);
}

std::queue<Message>& MessageBus::messages() {
    static std::queue<Message> messages;
    return messages;
}

MessageBus::SubscriberList& MessageBus::subscribers() {
    static SubscriberList subscribers;
    return subscribers;
}

std::vector<MessageBus::MessageFunc>& MessageBus::allSubscribers() {
    static std::vector<MessageFunc> allSubscribers;
    return allSubscribers;
}
}  // namespace Messages