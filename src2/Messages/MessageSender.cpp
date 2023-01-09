#include "MessageSender.h"

namespace Messages {
// MessageSenderBase
std::string MessageSenderBase::getType() const {
    std::string str = typeid(*this).name();
    return str.substr(std::find_if(str.begin(), str.end(), ::isalpha) -
                      str.begin());
}
}  // namespace Messages
