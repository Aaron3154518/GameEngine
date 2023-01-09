#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include <Messages/MessageBus.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace Services {
class CommandService {
   public:
    CommandService();

    bool checkInput();

   private:
};
}  // namespace Services

#endif
