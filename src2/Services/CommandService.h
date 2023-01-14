#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include <Components/NameComponent.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace Services {
class CommandService : public Service {
   public:
    bool checkInput();

   private:
    void init();
};
}  // namespace Services

#endif
