#ifndef MESSAGER_H
#define MESSAGER_H

#include <Entities/UUID.h>
#include <Messages/MessageBus.h>

#include <algorithm>
#include <functional>
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

    template <class MsgT>
    void subscribeTo(const std::function<void(const MsgT&)>& callback,
                     typename MsgT::Code code) {
        mSubscriptions.push_back(
            GetMessageBus().subscribe<MsgT>(callback, id(), code));
    }

    template <class MsgT>
    void subscribeTo(const std::function<void(const MsgT&)>& callback) {
        mSubscriptions.push_back(
            GetMessageBus().subscribe<MsgT>(callback, id()));
    }

    void startTimer(int len_ms, const std::function<void()>& callback);

   private:
    virtual void init();

    const Entities::UUID mId;

    std::vector<MessageHandle> mSubscriptions;
};

typedef std::unique_ptr<Messager> MessagerPtr;
}  // namespace Messages

#endif
