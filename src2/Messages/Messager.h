#ifndef MESSAGER_H
#define MESSAGER_H

#include <Entities/UUID.h>
#include <Messages/Message.h>
#include <Messages/MessageBus.h>

#include <algorithm>
#include <string>
#include <type_traits>
#include <vector>

namespace Messages {
class Messager {
   public:
    Messager();
    virtual ~Messager() = default;

    // No copying UUID
    Messager(const Messager& other) = delete;
    Messager& operator=(const Messager& other) = delete;

    operator Entities::UUID() const;
    Entities::UUID id() const;

   private:
    const Entities::UUID mId;
};

class Receiver : public virtual Messager {
   public:
    virtual ~Receiver();

   protected:
    void attachSubscription(const Messages::MessageHandle& handle);

   private:
    std::vector<MessageHandle> mSubscriptions;
};

template <class T>
class Sender : public virtual Messager {
    static_assert(std::is_enum<T>::value,
                  "Sender<>: Message type must be enum");

   public:
    virtual ~Sender() = default;

    void sendMessage(T t) const {
        GetMessageBus().queueMessage(std::make_unique<Message>(id(), t));
    }
};
}  // namespace Messages

#endif
