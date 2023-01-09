#include <Components/Component.h>
#include <Entities/Entity.h>
#include <Messages/MessageBus.h>
#include <Messages/MessageSender.h>
#include <Services/CommandService.h>
#include <Windows.h>

#include <iostream>

enum MyServiceMessage : Messages::EnumT { Hello = 0, World = 1 };

class MyService : public Messages::MessageSender<MyServiceMessage> {
   public:
   private:
};

class MyComponent : public Components::Component {};

class MyEntity : public Entities::Entity {
   public:
    MyEntity(MyService& s) {
        addComponent<Components::Component>();
        addComponent<MyComponent>();
        Messages::MessageBus::subscribe(s.getType(), MyServiceMessage::Hello,
                                        [](const Messages::Message& m) {
                                            std::cerr << "Hello" << std::endl;
                                        });
    }
};

DWORD WINAPI runCommand(LPVOID param) {
    Services::CommandService& cs = *(Services::CommandService*)param;
    while (cs.checkInput()) {
        Messages::MessageBus::sendMessages();
        Sleep(16);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // Enables colored text
    system("echo");

    MyService s;
    Services::CommandService cs;
    MyEntity e(s);

    s.sendMessage(MyServiceMessage::Hello);
    s.sendMessage(MyServiceMessage::World);

    Messages::MessageBus::sendMessages();

    DWORD comThreadId;
    HANDLE comThread = CreateThread(0, 0, runCommand, &cs, 0, &comThreadId);

    for (int i = 0; i < 10; i++) {
        Sleep(160);
    }

    std::cerr << "Hello World" << std::endl;

    WaitForSingleObject(comThread, INFINITE);
    CloseHandle(comThread);

    return 0;
}