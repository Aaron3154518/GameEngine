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

template <class...>
struct Message;

template <>
struct Message<> {
    Entities::UUID src;
    EnumT code;
    MessageOptions opts;

    Message(const Entities::UUID& s, EnumT c, const MessageOptions& o = {})
        : src(s), code(c), opts(o) {}
    virtual ~Message() = default;
};

template <class SrcT>
struct Message<SrcT> : public Message<> {
    Message(typename SrcT::Code c, const MessageOptions& o = {})
        : Message<>(GameObjects::Get<SrcT>().id(), c, o) {}

    static std::unique_ptr<Message> New(typename SrcT::Code c,
                                        const MessageOptions& o = {}) {
        return std::make_unique<Message>(c, o);
    }
};

template <class SrcT, class DataT>
struct Message<SrcT, DataT> : public Message<> {
    Message(const DataT& d, typename SrcT::Code c, const MessageOptions& o = {})
        : Message<>(GameObjects::Get<SrcT>().id(), c, o), data(d) {}

    static std::unique_ptr<Message> New(const DataT& d, typename SrcT::Code c,
                                        const MessageOptions& o = {}) {
        return std::make_unique<Message>(d, c, o);
    }

    DataT data;
};

typedef std::unique_ptr<Message<>> MessagePtr;
}  // namespace Messages

#endif
