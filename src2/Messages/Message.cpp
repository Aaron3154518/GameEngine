#include "Message.h"

namespace Messages {
std::unordered_map<std::string, MessageTypes::MsgGenFunc<>>&
MessageTypes::GenMap() {
    static std::unordered_map<std::string, MsgGenFunc<>> GEN_MAP;
    return GEN_MAP;
}

std::unordered_map<std::type_index, std::string>& MessageTypes::NameMap() {
    static std::unordered_map<std::type_index, std::string> NAME_MAP;
    return NAME_MAP;
}

Messages::MessagePtr MessageTypes::Generate(const std::string& name,
                                            Messages::EnumT code,
                                            const std::string& line) {
    auto it = GenMap().find(name);
    if (it == GenMap().end()) {
        return nullptr;
    }
    return it->second(code, line);
}

std::string MessageTypes::GetName(const std::type_index& id) {
    auto it = NameMap().find(id);
    return it == NameMap().end() ? id.name() : it->second;
}
}  // namespace Messages
