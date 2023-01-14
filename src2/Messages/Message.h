#ifndef MESSAGE_H
#define MESSAGE_H

#include <Entities/UUID.h>

#include <memory>
#include <string>

namespace Messages {
typedef Entities::UUID MessageT;
typedef int EnumT;

class Message {
   public:
    Message(const MessageT& type, EnumT code);
    virtual ~Message() = default;

    bool operator==(const Message& other);

    const MessageT& type() const;
    EnumT code() const;

    const Entities::UUID& target() const;
    void setTarget(const Entities::UUID& target);

   private:
    MessageT mType;
    EnumT mCode;
    Entities::UUID mTarget = {0};
};

typedef std::unique_ptr<Message> MessagePtr;
}  // namespace Messages

#endif
