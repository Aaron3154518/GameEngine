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
constexpr Entities::UUID NO_TYPE = {0};

struct MessageHandle {
    const Entities::UUID eId;
    const uint32_t eNum;
    const Entities::UUID type;
    const EnumT code;
};

class MessageBus {
    typedef std::function<void(const Message<>&)> MessageFunc;

    struct EntityCallback {
        Entities::UUID eId;
        uint32_t eNum;
        MessageFunc func;
    };

   public:
    void sendMessage(const Message<>& msg);

    void unsubscribe(MessageHandle handle);

   private:
    template <class>
    friend class Subscribe;
    friend MessageBus& GetMessageBus();

    MessageBus() = default;

    MessageHandle subscribe(
        const std::function<void(const Message<>&)>& callback,
        Entities::UUID eId, const Entities::UUID& msgType = NO_TYPE,
        EnumT msgCode = NO_CODE);

    void sendMessage(const Message<>& msg,
                     const std::vector<EntityCallback>& targets);

    typedef std::unordered_map<EnumT, std::vector<EntityCallback>>
        MessageSubscribers;
    std::unordered_map<Entities::UUID, MessageSubscribers> subscribers;
    std::unordered_map<Entities::UUID, uint32_t> entity_counts;
};

MessageBus& GetMessageBus();

template <class MsgT = Message<>>
class Subscribe;

template <>
class Subscribe<Message<>> {
    typedef Message<> MsgT;

    // Subscribe to all messages
    static MessageHandle subscribe(
        const Entities::UUID eId,
        const std::function<void(const MsgT&)>& callback) {
        return GetMessageBus().subscribe(callback, eId);
    }
};

template <class SrcT, class CodeT, class DataT>
class Subscribe<Message<SrcT, CodeT, DataT>> {
   public:
    typedef Message<SrcT, CodeT, DataT> MsgT;

    // Subscribe to all messages from sender
    static MessageHandle subscribe(
        const Entities::UUID eId,
        const std::function<void(const MsgT&)>& callback) {
        return subscribe(eId, callback, NO_CODE);
    }

    // Subscribe to messages from send with given code
    static MessageHandle subscribe(
        const Entities::UUID eId,
        const std::function<void(const MsgT&)>& callback, CodeT msgCode) {
        return subscribe(eId, callback, msgCode);
    }

    static const Entities::UUID& senderId() {
        return GameObjects::Get<SrcT>().id();
    }

   private:
    static MessageHandle subscribe(
        const Entities::UUID eId,
        const std::function<void(const MsgT&)>& callback, EnumT msgCode) {
        return GetMessageBus().subscribe(
            [callback](const Message<>& m) {
                auto msg = static_cast<const MsgT*>(&m);
                if (!msg) {
                    throw std::runtime_error(
                        "Error: Message could not be converted to type: " +
                        std::string(typeid(MsgT).name()));
                }
                callback(*msg);
            },
            eId, senderId(), msgCode);
    }
};
}  // namespace Messages

#endif
