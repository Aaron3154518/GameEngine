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
constexpr EnumT NO_CODE = -1;
constexpr MessageT NO_TYPE = {0};

struct MessageHandle {
    const Entities::UUID eId;
    const uint32_t eNum;
    const MessageT type;
    const EnumT code;
};

class MessageBus {
    typedef std::function<void(const Message*)> MessageFunc;

    struct EntityCallback {
        Entities::UUID eId;
        uint32_t eNum;
        MessageFunc func;
    };

   public:
    // static const MessageT& NO_TYPE();

    void queueMessage(MessagePtr msg);
    void sendImmediateMessage(MessagePtr msg);

    void sendMessages();

    MessageHandle subscribe(const std::function<void(const Message&)>& callback,
                            Entities::UUID eId,
                            const MessageT& msgType = NO_TYPE,
                            EnumT msgCode = NO_CODE);

    template <class MsgT>
    MessageHandle subscribe(const std::function<void(const MsgT&)>& callback,
                            Entities::UUID eId,
                            const MessageT& msgType = NO_TYPE,
                            EnumT msgCode = NO_CODE) {
        static_assert(
            std::is_base_of<Message, MsgT>::value,
            "MessageBus::subscribe(): Message type must derive from Message");
        return subscribe(
            [callback](const Message* m) {
                const MsgT* msg = static_cast<const MsgT*>(m);
                if (!msg) {
                    throw std::runtime_error(
                        "Error: Message could not be converted to type: " +
                        std::string(typeid(MsgT).name()));
                }
                callback(*msg);
            },
            eId, msgType, msgCode);
    }

    void unsubscribe(MessageHandle handle);

   private:
    MessageHandle subscribe(const MessageFunc& callback, Entities::UUID eId,
                            const MessageT& msgType, EnumT msgCode);

    friend MessageBus& GetMessageBus();
    MessageBus() = default;

    void sendMessage(const MessagePtr& msg);
    void sendMessage(const MessagePtr& msg,
                     const std::vector<EntityCallback>& targets);

    std::queue<MessagePtr> messages;

    typedef std::unordered_map<EnumT, std::vector<EntityCallback>>
        MessageSubscribers;
    std::unordered_map<MessageT, MessageSubscribers> subscribers;
    std::unordered_map<Entities::UUID, uint32_t> entity_counts;
};

MessageBus& GetMessageBus();
}  // namespace Messages

#endif
