#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

#include <Entities/UUID.h>
#include <Messages/Message.h>

#include <functional>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

namespace Messages {
struct MessageHandle {
    const Entities::UUID eId;
    const uint32_t eNum;
    const MessageT type;
    const EnumT code;
};

class MessageBus {
    typedef std::function<void(const Message&)> MessageFunc;

    struct EntityCallback {
        Entities::UUID eId;
        uint32_t eNum;
        MessageFunc func;
    };

   public:
    void queueMessage(MessagePtr msg);
    void sendImmediateMessage(MessagePtr msg);

    void sendMessages();

    MessageHandle subscribe(Entities::UUID eId, const MessageT& msgType,
                            EnumT msgCode, const MessageFunc& callback);
    MessageHandle subscribe(Entities::UUID eId, const MessageT& msgType,
                            const MessageFunc& callback);
    MessageHandle subscribe(Entities::UUID eId, const MessageFunc& callback);

    void unsubscribe(MessageHandle handle);

   private:
    friend MessageBus& GetMessageBus();
    MessageBus() = default;

    void sendMessage(const MessagePtr& msg);

    std::queue<MessagePtr> messages;

    typedef std::unordered_map<EnumT, std::vector<EntityCallback>>
        MessageSubscribers;
    std::unordered_map<MessageT, MessageSubscribers> subscribers;
    std::unordered_map<Entities::UUID, uint32_t> entity_counts;
};

MessageBus& GetMessageBus();
}  // namespace Messages

#endif
