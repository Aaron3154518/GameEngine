#ifndef MESSAGE_RECEIVER_H
#define MESSAGE_RECEIVER_H

#include <Entities/UUID.h>
#include <Messages/MessageBus.h>

namespace Messages {
class MessageReceiver {
   public:
    MessageReceiver();
    virtual ~MessageReceiver();

    operator Entities::UUID() const;
    Entities::UUID id() const;

   protected:
    void attachSubscription(const Messages::MessageHandle& handle);

   private:
    const Entities::UUID mId;
    std::vector<MessageHandle> mSubscriptions;
};
}  // namespace Messages

#endif
