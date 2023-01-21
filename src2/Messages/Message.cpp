#include "Message.h"

namespace Messages {
BaseMessageData::BaseMessageData(MessageT t, EnumT c, MessageOptions o)
    : type(t), code(c), opts(o) {}
}  // namespace Messages
