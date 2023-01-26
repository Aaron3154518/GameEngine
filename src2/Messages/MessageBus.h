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

    template <class MsgT>
    MessageHandle subscribe(const std::function<void(const MsgT&)>& callback,
                            const Entities::UUID& eId,
                            typename MsgT::CodeT msgCode) {
        return subscribe(
            [callback](const auto& m) { callback(convert<MsgT>(m)); }, eId,
            MsgT::ID(), msgCode);
    }

    template <class MsgT>
    MessageHandle subscribe(const std::function<void(const MsgT&)>& callback,
                            const Entities::UUID& eId) {
        return subscribe(
            [callback](const auto& m) { callback(convert<MsgT>(m)); }, eId,
            MsgT::ID(), NO_CODE);
    }

   private:
    friend class Messager;
    friend MessageBus& GetMessageBus();

    MessageBus() = default;

    MessageHandle subscribe(
        const std::function<void(const Message<>&)>& callback,
        const Entities::UUID& eId, const Entities::UUID& mId, EnumT msgCode);

    void sendMessage(const Message<>& msg,
                     const std::vector<EntityCallback>& targets);

    typedef std::unordered_map<EnumT, std::vector<EntityCallback>>
        MessageSubscribers;
    std::unordered_map<Entities::UUID, MessageSubscribers> subscribers;
    std::unordered_map<Entities::UUID, uint32_t> entity_counts;
};

MessageBus& GetMessageBus();
}  // namespace Messages

#endif
