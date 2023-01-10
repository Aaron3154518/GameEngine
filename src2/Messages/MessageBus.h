#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

#include <Messages/Message.h>

#include <functional>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

namespace Messages {
class MessageBus {
    typedef std::function<void(const Message&)> MessageFunc;

    class MessageSubscribers {
       public:
        void sendMessage(MessagePtr& msg);

        void subscribe(EnumT code, const MessageFunc& callback);
        void subscribe(const MessageFunc& callback);

       private:
        std::vector<MessageFunc> mAllSubscribers;
        std::unordered_map<EnumT, std::vector<MessageFunc>> mSubscribers;
    };

   public:
    static void queueMessage(MessagePtr msg);

    static void sendMessages();

    static void subscribe(const MessageT& msgType, EnumT msgCode,
                          const MessageFunc& callback);

    static void subscribe(const MessageT& msgType, const MessageFunc& callback);

    static void subscribe(const MessageFunc& callback);

   private:
    static std::queue<MessagePtr>& messages();

    typedef std::unordered_map<MessageT, MessageSubscribers> SubscriberList;
    static SubscriberList& subscribers();

    static std::vector<MessageFunc>& allSubscribers();
};
}  // namespace Messages

#endif
