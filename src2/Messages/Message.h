#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

namespace Messages {
typedef std::string MessageT;
typedef int EnumT;

class Message {
   public:
    Message(const MessageT& type, EnumT code);
    virtual ~Message() = default;

    bool operator==(const Message& other);

    const MessageT& type() const;
    EnumT code() const;

   private:
    MessageT mType;
    EnumT mCode;
};
}  // namespace Messages

#endif
