#include "Message.h"

namespace Messages {
// Message
Message::Message(const MessageT& type, EnumT code) : mType(type), mCode(code) {}

bool Message::operator==(const Message& other) {
    return mType == other.mType && mCode == other.mCode;
}

const MessageT& Message::type() const { return mType; }
EnumT Message::code() const { return mCode; }
}  // namespace Messages
