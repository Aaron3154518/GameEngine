#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include <Components/Component.h>
#include <Components/NameComponent.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>
#include <Windows.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Services {
class CommandComponent
    : public Components::DataComponent<std::vector<Messages::EnumT>> {
   public:
    using Components::DataComponent<
        std::vector<Messages::EnumT>>::DataComponent;
};
typedef Components::ComponentManager<CommandComponent> CommandComponentManager;

class CommandService : public Service {
   public:
    struct CommandData {
        std::string line;
        Messages::EnumT cmdCode;
    };

    MESSAGE(Message, Quit);

    bool checkInput(std::queue<Messages::MessagePtr>& msgs,
                    CRITICAL_SECTION* msgQueue);

   private:
    void service_init();

   public:
    template <class MsgT = Messages::Message<>>
    using MsgGenFunc = std::function<std::unique_ptr<MsgT>(typename MsgT::Code,
                                                           const std::string&)>;

    template <class MsgT>
    static bool Register(const std::string& name, const MsgGenFunc<MsgT>& gen) {
        auto& msgName = NameMap()[Messages::ID<MsgT>()];
        if (!msgName.empty()) {
            std::cerr << "MessageTypes::Register(): Duplicate message name \""
                      << name << "\"" << std::endl;
        }
        msgName = name;
        GenMap()[name] = [gen](Messages::EnumT code, const std::string& line) {
            return gen(static_cast<typename MsgT::Code>(code), line);
        };
        return true;
    }

    static Messages::MessagePtr Generate(const std::string& name,
                                         Messages::EnumT code,
                                         const std::string& line);

    static std::string GetName(const std::type_index& id);

   private:
    static std::unordered_map<std::string, MsgGenFunc<>>& GenMap();

    static std::unordered_map<std::type_index, std::string>& NameMap();
};

template <class MsgT, const char* Name>
struct Register {
    static const bool _;

    using Msg = MsgT;
    using MsgPtr = std::unique_ptr<Msg>;
    using MsgCode = typename Msg::Code;
    using MsgData = typename Msg::Data;

    static MsgPtr parse(MsgCode c, const std::string& line) {
        return std::make_unique<Msg>(c);
    }
};

template <class MsgT, const char* Name>
const bool Register<MsgT, Name>::_ =
    CommandService::Register<MsgT>(Name, Register<MsgT, Name>::parse);
}  // namespace Services

// Macros to help register message types
#define SELECT(_1, _2, _3, foo, ...) foo

#define REGISTER_2(msg, name)            \
    constexpr char name##Name[] = #name; \
    template class Services::Register<msg, name##Name>

#define REGISTER_1(msg, name, body)                                  \
    constexpr char name##Name[] = #name;                             \
    template <>                                                      \
    std::unique_ptr<msg> Services::Register<msg, name##Name>::parse( \
        MsgCode code, const std::string& line)                       \
        body template class Services::Register<msg, name##Name>

#define REGISTER(...) SELECT(__VA_ARGS__, REGISTER_1, REGISTER_2)(__VA_ARGS__)

#endif
