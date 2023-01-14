#ifndef SERVICE_H
#define SERVICE_H

#include <Messages/Messager.h>

namespace Services {
template <class T = void>
class Service : public Messages::Receiver, public Messages::Sender<T> {
   public:
    virtual ~Service() = default;
};

template <>
class Service<> : public Messages::Receiver {
   public:
    virtual ~Service() = default;
};

}  // namespace Services

#endif
