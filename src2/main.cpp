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

class MyComponent : public Components::Component {
   public:
    void onHello() { std::cerr << "Hello" << std::endl; }
    void onWorld() { std::cerr << "World" << std::endl; }
};

class MyComponentManager : public Components::ComponentManager<MyComponent> {
   public:
    MyComponentManager() {
        Messages::MessageBus::subscribe(
            MyService{}.getType(),
            [this](const Messages::Message& m) { onMyServiceMessage(m); });
    }

    void onMyServiceMessage(const Messages::Message& m) {
        switch (m.code()) {
            case MyServiceMessage::Hello:
                for (auto& comp : *this) {
                    comp.onHello();
                }
                break;
            case MyServiceMessage::World:
                for (auto& comp : *this) {
                    comp.onWorld();
                }
                break;
            default:
                break;
        };
    }
};

class MyEntity : public Entities::Entity {
   public:
    MyEntity(const MyService& s) {
        addComponent<Components::ComponentManager<Components::Component>>();
        addComponent<MyComponentManager>();
        Messages::MessageBus::subscribe(
            s.getType(), MyServiceMessage::Hello,
            [s](const Messages::Message& m) {
                s.sendMessage(MyServiceMessage::World);
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

    WaitForSingleObject(comThread, INFINITE);
    CloseHandle(comThread);

    return 0;
}