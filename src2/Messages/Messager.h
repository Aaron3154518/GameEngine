#ifndef MESSAGER_H
#define MESSAGER_H

#include <Entities/UUID.h>
#include <Messages/Message.h>
#include <Messages/MessageBus.h>

#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// Forward declaration
class GameObjects;

namespace Messages {
class Messager {
    friend class ::GameObjects;

   public:
    Messager();
    virtual ~Messager();

    // No copying UUID
    Messager(const Messager& other) = delete;
    Messager& operator=(const Messager& other) = delete;

    operator Entities::UUID() const;
    Entities::UUID id() const;

    void setName(const std::string& name) const;

   protected:
    virtual void init();

    void attachSubscription(const Messages::MessageHandle& handle);

   private:
    const Entities::UUID mId;

    std::vector<MessageHandle> mSubscriptions;
};

typedef std::unique_ptr<Messager> MessagerPtr;
}  // namespace Messages

#endif
