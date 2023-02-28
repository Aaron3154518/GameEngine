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

// Macros to help register message types with cli
#define SELECT(_1, _2, _3, foo, ...) foo

#define REGISTER_2(msg, name)            \
    constexpr char name##Name[] = #name; \
    template class Messages::MessageTypes::Register<msg, name##Name>

#define REGISTER_1(msg, name, body)                           \
    constexpr char name##Name[] = #name;                      \
    template <>                                               \
    std::unique_ptr<msg>                                      \
    Messages::MessageTypes::Register<msg, name##Name>::parse( \
        MsgCode code, const std::string& line)                \
        body template class Messages::MessageTypes::Register<msg, name##Name>

#define REGISTER(...) SELECT(__VA_ARGS__, REGISTER_1, REGISTER_2)(__VA_ARGS__)

namespace Messages {
typedef int EnumT;

template <class T>
std::type_index ID() {
    return std::type_index(typeid(T));
}

// Message classes
struct MessageOptions {
    Entities::UUID target = Entities::NullId();
    bool quiet = true;
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

    virtual std::type_index id() const {
        return std::type_index(typeid(*this));
    }

   protected:
    Message(EnumT c, const MessageOptions& o = {}) : code(c), opts(o) {}
};

template <class CodeT>
struct Message<CodeT, void> : public Message<> {
    using Code = CodeT;
    using Data = void;

    Message(Code c, const MessageOptions& o = {}) : Message<>(c, o) {}
    Message(std::type_index id, Code c, const MessageOptions& o = {})
        : Message<>(c, o), mId(id) {}

    std::type_index id() const {
        return mId == std::type_index(typeid(Message<>)) ? Message<>::id()
                                                         : mId;
    }

    std::type_index mId = std::type_index(typeid(Message<>));
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

// Map message types to command line
class MessageTypes {
   private:
    template <class MsgT = Messages::Message<>>
    using MsgGenFunc = std::function<std::unique_ptr<MsgT>(typename MsgT::Code,
                                                           const std::string&)>;

    static std::unordered_map<std::string, MsgGenFunc<>>& GenMap();

    static std::unordered_map<std::type_index, std::string>& NameMap();

   public:
    // Class to help register message types
    template <class MsgT, const char* Name>
    struct Register {
       public:
        static const bool _;

        using Msg = MsgT;
        using MsgPtr = std::unique_ptr<Msg>;
        using MsgCode = typename Msg::Code;
        using MsgData = typename Msg::Data;

        static MsgPtr parse(MsgCode c, const std::string& line) {
            return std::make_unique<Msg>(c);
        }

       private:
        static bool add() {
            auto& msgName = MessageTypes::NameMap()[Messages::ID<MsgT>()];
            if (!msgName.empty()) {
                std::cerr
                    << "MessageTypes::Register(): Duplicate message name \""
                    << Name << "\"" << std::endl;
            }
            msgName = Name;
            MessageTypes::GenMap()[msgName] = [](Messages::EnumT code,
                                                 const std::string& line) {
                return parse(static_cast<typename MsgT::Code>(code), line);
            };
            return true;
        }
    };

    static Messages::MessagePtr Generate(const std::string& name,
                                         Messages::EnumT code,
                                         const std::string& line);

    static std::string GetName(const std::type_index& id);
};

template <class MsgT, const char* Name>
const bool MessageTypes::Register<MsgT, Name>::_ =
    MessageTypes::Register<MsgT, Name>::add();

// Helper function
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
