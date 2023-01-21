#ifndef MESSAGE_H
#define MESSAGE_H

#include <Entities/UUID.h>
#include <Messages/GameObjects.h>

#include <iostream>
#include <memory>
#include <string>

namespace Messages {
typedef Entities::UUID MessageT;
typedef int EnumT;

struct MessageOptions {
    Entities::UUID target = {0};
};

struct BaseMessageData {
    MessageT type;
    EnumT code;
    MessageOptions opts;

    BaseMessageData(MessageT t, EnumT c, MessageOptions o = {});
    virtual ~BaseMessageData() = default;
};

template <class MsgT, class EnT>
struct MessageData : public BaseMessageData {
    // static_assert(std::is_integral<EnT>::value,
    //"MessageData<>: Enum type must be integral type");

    MessageData(EnT code, const MessageOptions& opts = {})
        : BaseMessageData(GameObjects::Get<MsgT>().id(), code, opts) {}
    operator BaseMessageData() const {
        std::cerr << "Convert" << std::endl;
        return BaseMessageData{type, code, opts};
    }

    virtual ~MessageData() = default;
};

struct BaseMessage {
    // TODO: try w/o const &
    BaseMessage(const BaseMessageData& msg) : data(msg) {}
    virtual ~BaseMessage() = default;

    BaseMessageData data;
};

template <class MsgT, class EnT>
struct Message : public BaseMessage {
    typedef EnT EnumType;
    typedef MessageData<MsgT, EnT> MessageData;

    Message(const MessageData& msg) : BaseMessage(msg) {}
    virtual ~Message() = default;
};

typedef std::unique_ptr<BaseMessage> MessagePtr;

template <class T, class... ArgTs>
std::unique_ptr<T> make_unique_msg(typename T::EnumType code,
                                   const MessageOptions& opts,
                                   ArgTs&&... args) {
    return std::make_unique<T>(typename T::MessageData(code, opts),
                               std::forward<ArgTs>(args)...);
}

template <class T>
std::unique_ptr<T> make_unique_msg(typename T::EnumType code) {
    return std::make_unique<T>(typename T::MessageData(code));
}
}  // namespace Messages

#endif
