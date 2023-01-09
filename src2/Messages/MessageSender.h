#ifndef MESSAGE_SENDER_H
#define MESSAGE_SENDER_H

#include <Messages/MessageBus.h>

#include <algorithm>
#include <string>
#include <type_traits>

namespace Messages {
class MessageSenderBase {
   public:
    virtual ~MessageSenderBase() = default;

    std::string getType();

   private:
    std::string className;
};

template <class T>
class MessageSender : public MessageSenderBase {
    static_assert(std::is_enum<T>::value,
                  "MessageSender<>: Message type must be enum");

   public:
    virtual ~MessageSender() = default;

    void sendMessage(T t) { MessageBus::queueMessage(Message(getType(), t)); }
};
}  // namespace Messages

#endif
