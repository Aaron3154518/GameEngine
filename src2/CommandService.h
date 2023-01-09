#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "Message.h"

class CommandService {
   public:
    CommandService() {
        MessageBus::subscribe([](const Message& msg) {
            std::cerr << "\033[1;34m[Message]\033[0m " << msg.type() << " "
                      << msg.code() << std::endl;
        });
    }

    bool checkInput() {
        char* str = (char*)malloc(sizeof(char) * 255);
        std::cin.getline(str, 255);
        std::stringstream ss(str);
        free(str);

        std::string service;
        ss >> service;

        if (service == "quit") {
            return false;
        }

        if (service == "MyService") {
            int code;
            if (ss >> code) {
                MessageBus::queueMessage(Message(service, code));
            }
        }

        return true;
    }

   private:
};

#endif
