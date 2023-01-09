#include "MessageSender.h"

namespace Messages {
// MessageSenderBase
std::string MessageSenderBase::getType() {
    if (className.empty()) {
        className = typeid(*this).name();
        className.erase(className.begin(),
                        std::find_if(std::begin(className), std::end(className),
                                     ::isalpha));
    }
    return className;
}
}  // namespace Messages
