#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

#include <Messages/Message.h>

#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>

namespace Messages {
class MessageBus {
    typedef std::function<void(const Message&)> MessageFunc;

    class MessageSubscribers {
       public:
        void sendMessage(const Message& msg);

        void subscribe(EnumT code, const MessageFunc& callback);
        void subscribe(const MessageFunc& callback);

       private:
        std::vector<MessageFunc> mAllSubscribers;
        std::unordered_map<EnumT, std::vector<MessageFunc>> mSubscribers;
    };

   public:
    static void queueMessage(const Message& msg);

    static void sendMessages();

    static void subscribe(const MessageT& msgType, EnumT msgCode,
                          const MessageFunc& callback);

    static void subscribe(const MessageT& msgType, const MessageFunc& callback);

    static void subscribe(const MessageFunc& callback);

   private:
    static std::queue<Message>& messages();

    typedef std::unordered_map<MessageT, MessageSubscribers> SubscriberList;
    static SubscriberList& subscribers();

    static std::vector<MessageFunc>& allSubscribers();
};
}  // namespace Messages

#endif
