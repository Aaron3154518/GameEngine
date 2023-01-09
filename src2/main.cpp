#include <iostream>

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
        MessageBus::subscribe(s.getType(), [](const Message& m) {
            std::cerr << m.type() << " " << m.code() << std::endl;
        });
    }
};

int main(int argc, char* argv[]) {
    MyService s;
    MyEntity e(s);

    s.sendMessage(MyServiceMessage::Hello);
    s.sendMessage(MyServiceMessage::World);

    MessageBus::sendMessages();

    std::cerr << "Hello World" << std::endl;
    return 0;
}