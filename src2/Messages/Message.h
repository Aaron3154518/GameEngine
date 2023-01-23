#ifndef MESSAGE_H
#define MESSAGE_H

#include <Entities/UUID.h>
#include <Messages/GameObjects.h>

#include <iostream>
#include <memory>
#include <string>

namespace Messages {
typedef int EnumT;

struct MessageOptions {
    Entities::UUID target = {0};
};

template <class SrcT = void, class CodeT = void, class DataT = void>
struct Message;

template <class SrcT>
struct Message<SrcT, void, void> {
    Entities::UUID src;
    EnumT code;
    MessageOptions opts;

    Message(const Entities::UUID& s, EnumT c, const MessageOptions& o = {})
        : src(s), code(c), opts(o) {}
    virtual ~Message() = default;
};

template <class SrcT, class CodeT>
struct Message<SrcT, CodeT, void> : public Message<> {
    Message(CodeT c, const MessageOptions& o = {})
        : Message<>(GameObjects::Get<SrcT>().id(), c, o) {}

    static std::unique_ptr<Message> New(CodeT c, const MessageOptions& o = {}) {
        return std::make_unique<Message>(c, o);
    }
};

template <class SrcT, class CodeT, class DataT>
struct Message {
    Message(const DataT& d, CodeT c, const MessageOptions& o = {})
        : Message<>(GameObjects::Get<SrcT>().id(), c, o), data(d) {}

    static std::unique_ptr<Message> New(const DataT& d, CodeT c,
                                        const MessageOptions& o = {}) {
        return std::make_unique<Message>(d, c, o);
    }

    DataT data;
};

typedef std::unique_ptr<Message<>> MessagePtr;
}  // namespace Messages

#endif
