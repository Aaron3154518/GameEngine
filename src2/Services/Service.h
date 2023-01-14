#ifndef SERVICE_H
#define SERVICE_H

#include <Messages/Messager.h>

namespace Services {
class Service : public Messages::Messager {
   public:
    virtual ~Service() = default;
};
}  // namespace Services

#endif
