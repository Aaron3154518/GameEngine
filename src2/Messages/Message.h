#ifndef MESSAGE_H
#define MESSAGE_H

#include <Entities/UUID.h>
#include <Messages/GameObjects.h>

#include <iostream>
#include <memory>
#include <string>

// Macros to help expedite message declaring
#define MESSAGE(name, code1, ...)                                \
    enum name##Code : Messages::EnumT{##code1 = 0, __VA_ARGS__}; \
    typedef Messages::Message<name##Code> name

#define MESSAGE_D(name, data, code1, ...)                        \
    enum name##Code : Messages::EnumT{##code1 = 0, __VA_ARGS__}; \
    typedef Messages::Message<name##Code, data> name

namespace Messages {
typedef int EnumT;

struct MessageOptions {
    Entities::UUID target = {0};
};

template <class CodeT = void, class DataT = void>
struct Message;

template <>
struct Message<void, void> {
    virtual const Entities::UUID& id() const { return Entities::NullId(); }

    const EnumT code;
    const MessageOptions opts;

    Message(EnumT c, const MessageOptions& o = {}) : code(c), opts(o) {}
    virtual ~Message() = default;
};

template <class CodeT>
struct Message<CodeT, void> : public Message<> {
    using CodeT = CodeT;

    static const Entities::UUID& ID() {
        static const Entities::UUID ID = Entities::generateUUID();
        return ID;
    }
    const Entities::UUID& id() const { return ID(); }

    Message(CodeT c, const MessageOptions& o = {}) : Message<>(c, o) {}
    virtual ~Message() = default;

    static std::unique_ptr<Message> New(CodeT c, const MessageOptions& o = {}) {
        return std::make_unique<Message>(c, o);
    }

    static std::unique_ptr<Message> Parse(EnumT c, const std::string& line) {
        return Message<Name, CodeT>::New(static_cast<CodeT>(c));
    }
};

template <class CodeT, class DataT>
struct Message : public Message<> {
    using CodeT = CodeT;
    using DataT = DataT;

    static const Entities::UUID& ID() {
        static const Entities::UUID ID = Entities::generateUUID();
        return ID;
    }
    const Entities::UUID& id() const { return ID(); }

    DataT data;

    Message(const DataT& d, CodeT c, const MessageOptions& o = {})
        : Message<>(c, o), data(d) {}
    virtual ~Message() = default;

    static std::unique_ptr<Message> New(const DataT& d, CodeT c,
                                        const MessageOptions& o = {}) {
        return std::make_unique<Message>(d, c, o);
    }

    static std::unique_ptr<Message> Parse(EnumT c, const std::string& line) {
        return Message<Name, CodeT, DataT>::New(DataT(), static_cast<CodeT>(c));
    }
};

typedef std::unique_ptr<Message<>> MessagePtr;

class MessageTypes {
   private:
    template <class MsgT = Message<>>
    using MsgGenFunc =
        std::function<std::unique_ptr<MsgT>(EnumT, const std::string&)>;
    static std::unordered_map<std::string, MsgGenFunc<>>& GenMap() {
        static std::unordered_map<std::string, MsgGenFunc<>> GEN_MAP;
        return GEN_MAP;
    }

    static std::unordered_map<Entities::UUID, std::string> NameMap() {
        static std::unordered_map<Entities::UUID, std::string> NAME_MAP;
        return NAME_MAP;
    }

   public:
    template <class MsgT>
    static void Register(const std::string& name,
                         const MsgGenFunc<MsgT>& gen = MsgT::Parse) {
        auto& msgName = NameMap()[MsgT::ID()];
        if (!msgName.empty()) {
            std::cerr << "Warning: MessageTypes::Register(): Duplicate message "
                         "name \""
                      << name << "\"" << std::endl;
        }
        mgsName = name;
        GenMap()[name] = [gen](EnumT code, const std::string& line) {
            return gen(code, line);
        };
    }

    static MessagePtr Generate(const std::string& name, EnumT code,
                               const std::string& line) {
        auto it = GenMap().find(name);
        if (it == GenMap().end()) {
            return nullptr;
        }
        return it->second(code, line);
    }

    static std::string GetName(const Entities::UUID& id) {
        auto it = NameMap().find(id);
        if (it == NameMap().end()) {
            return std::to_string(id);
        }
        return it->second;
    }
};

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
