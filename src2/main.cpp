#include <Windows.h>

#include <iostream>

#include "CommandService.h"
#include "Component.h"
#include "Entity.h"
#include "Service.h"

enum MyServiceMessage : EnumT { Hello = 0, World = 1 };

class MyService : public Service, public MessageSender<MyServiceMessage> {
   public:
   private:
};

class MyComponent : public Component {};

class MyEntity : public Entity {
   public:
    MyEntity(MyService& s) {
        addComponent<Component>();
        addComponent<MyComponent>();
        MessageBus::subscribe(
            s.getType(), MyServiceMessage::Hello,
            [](const Message& m) { std::cerr << "Hello" << std::endl; });
    }
};

DWORD WINAPI runCommand(LPVOID param) {
    CommandService& cs = *(CommandService*)param;
    while (cs.checkInput()) {
        MessageBus::sendMessages();
        Sleep(16);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // Enables colored text
    system("echo");

    MyService s;
    CommandService cs;
    MyEntity e(s);

    s.sendMessage(MyServiceMessage::Hello);
    s.sendMessage(MyServiceMessage::World);

    MessageBus::sendMessages();

    DWORD myThreadId;
    HANDLE myHandle = CreateThread(0, 0, runCommand, &cs, 0, &myThreadId);

    for (int i = 0; i < 10; i++) {
        Sleep(1600);
    }

    std::cerr << "Hello World" << std::endl;

    WaitForSingleObject(myHandle, INFINITE);
    return 0;
}