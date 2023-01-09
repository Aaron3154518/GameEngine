#include <iostream>

#include "Component.h"
#include "Entity.h"

class MyComponent : public Component {};

class MyEntity : public Entity {
   public:
    MyEntity() {
        addComponent<Component>();
        addComponent<MyComponent>();
    }
};

int main(int argc, char* argv[]) {
    MyEntity e;

    std::cerr << "Hello World" << std::endl;
    return 0;
}