#ifndef MESSAGE_H
#define MESSAGE_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <type_traits>
#include <unordered_map>

typedef std::string MessageT;
typedef int EnumT;

class Message {
   public:
    Message(const MessageT& type, EnumT code) : mType(type), mCode(code) {}
    virtual ~Message() = default;

    bool operator==(const Message& other) {
        return mType == other.mType && mCode == other.mCode;
    }

    const MessageT& type() const { return mType; }
    EnumT code() const { return mCode; }

   private:
    MessageT mType;
    EnumT mCode;
};

typedef std::function<void(const Message&)> MessageFunc;

class MessageSubscribers {
   public:
    void sendMessage(const Message& msg) {
        for (auto& func : mAllSubscribers) {
            func(msg);
        }
        for (auto& func : mSubscribers[msg.code()]) {
            func(msg);
        }
    }

    void subscribe(EnumT code, const MessageFunc& callback) {
        mSubscribers[code].push_back(callback);
    }
    void subscribe(const MessageFunc& callback) {
        mAllSubscribers.push_back(callback);
    }

   private:
    std::vector<MessageFunc> mAllSubscribers;
    std::unordered_map<EnumT, std::vector<MessageFunc>> mSubscribers;
};

// Entry point
class MessageBus {
   public:
    static void queueMessage(const Message& msg) { messages().push(msg); }

    static void sendMessages() {
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

    static void subscribe(const MessageT& msgType, EnumT msgCode,
                          const MessageFunc& callback) {
        subscribers()[msgType].subscribe(msgCode, callback);
    }

    static void subscribe(const MessageT& msgType,
                          const MessageFunc& callback) {
        subscribers()[msgType].subscribe(callback);
    }

    static void subscribe(const MessageFunc& callback) {
        allSubscribers().push_back(callback);
    }

   private:
    static std::queue<Message>& messages() {
        static std::queue<Message> messages;
        return messages;
    }

    typedef std::unordered_map<MessageT, MessageSubscribers> SubscriberList;
    static SubscriberList& subscribers() {
        static SubscriberList subscribers;
        return subscribers;
    }

    static std::vector<MessageFunc>& allSubscribers() {
        static std::vector<MessageFunc> allSubscribers;
        return allSubscribers;
    }
};

template <class T>
class MessageSender {
    static_assert(std::is_enum<T>::value,
                  "MessageSender<>: Message type must be enum");

   public:
    virtual ~MessageSender() = default;

    void sendMessage(T t) { MessageBus::queueMessage(Message(getType(), t)); }

    std::string getType() {
        if (className.empty()) {
            className = typeid(*this).name();
            className.erase(className.begin(),
                            std::find_if(std::begin(className),
                                         std::end(className), ::isalpha));
        }
        return className;
    }

   private:
    std::string className;
};

#endif
