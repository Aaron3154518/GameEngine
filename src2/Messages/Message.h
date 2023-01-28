#ifndef MESSAGE_H
#define MESSAGE_H

#include <Entities/UUID.h>

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

// Macros to help expedite message declaring
#define MESSAGE_ENUM(name, code1, ...) \
    enum name : Messages::EnumT { code1 = 0, __VA_ARGS__ }

#define MESSAGE(name, code1, ...)                 \
    MESSAGE_ENUM(name##Code, code1, __VA_ARGS__); \
    typedef Messages::Message<name##Code> name

#define DATA_MESSAGE(name, data, code1, ...)      \
    MESSAGE_ENUM(name##Code, code1, __VA_ARGS__); \
    typedef Messages::Message<name##Code, data> name

namespace Messages {
typedef int EnumT;

template <class T>
std::type_index ID() {
    return std::type_index(typeid(T));
}

struct MessageOptions {
    Entities::UUID target = Entities::NullId();
};

template <class CodeT = void, class DataT = void>
struct Message;

template <>
struct Message<void, void> {
    using Code = EnumT;
    using Data = void;

    const EnumT code;
    const MessageOptions opts;

    virtual ~Message() = default;

    std::type_index id() const { return std::type_index(typeid(*this)); }

   protected:
    Message(EnumT c, const MessageOptions& o = {}) : code(c), opts(o) {}
};

template <class CodeT>
struct Message<CodeT, void> : public Message<> {
    using Code = CodeT;
    using Data = void;

    Message(Code c, const MessageOptions& o = {}) : Message<>(c, o) {}
};

template <class CodeT, class DataT>
struct Message : public Message<> {
    using Code = CodeT;
    using Data = DataT;

    Data data;

    Message(Code c, const Data& d, const MessageOptions& o = {})
        : Message<>(c, o), data(d) {}
};

typedef std::unique_ptr<Message<>> MessagePtr;

template <class MsgT>
typename std::enable_if<!std::is_same<Message<>, MsgT>::value,
                        const MsgT&>::type
convert(const Message<>& m) {
    auto msg = static_cast<const MsgT*>(&m);
    if (!msg) {
        throw std::runtime_error(
            "Error: Message could not be converted to type: " +
            std::string(typeid(MsgT).name()));
    }
    return *msg;
}

template <class MsgT>
typename std::enable_if<std::is_same<Message<>, MsgT>::value,
                        const Message<>&>::type
convert(const Message<>& m) {
    return m;
}
}  // namespace Messages

#endif
