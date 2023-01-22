#ifndef MESSAGER_H
#define MESSAGER_H

#include <Entities/UUID.h>

#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// Forward declaration
class GameObjects;

namespace Messages {
// Forward declaration
class MessageHandle;

class Messager {
    friend class ::GameObjects;

   public:
    // Descendants control the possible message codes by redefining Code
    // (e.g. with an enum)
    typedef int Code;

    Messager();
    virtual ~Messager();

    // No copying UUID
    Messager(const Messager& other) = delete;
    Messager& operator=(const Messager& other) = delete;

    operator Entities::UUID() const;
    Entities::UUID id() const;

    void setName(const std::string& name) const;

   protected:
    void attachSubscription(const Messages::MessageHandle& handle);

   private:
    virtual void init();

    const Entities::UUID mId;

    std::vector<MessageHandle> mSubscriptions;
};

typedef std::unique_ptr<Messager> MessagerPtr;
}  // namespace Messages

#endif
