#ifndef SERVICE_H
#define SERVICE_H

#include <Messages/MessageReceiver.h>

namespace Services {
class Service : public Messages::MessageReceiver {
   public:
    virtual ~Service() = default;

   protected:
   private:
};
}  // namespace Services

#endif
