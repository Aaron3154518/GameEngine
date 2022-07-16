#define SDL_MAIN_HANDLED

#include <Utils/Position.h>

#include <iostream>

void constructor_test()
{
    std::cerr << "Testing Constructor" << std::endl;
    Rect r(0, 0, 500, 500);
    std::cerr << r.toString() << std::endl;
    std::cerr << r.ToString() << std::endl;

    std::cerr << std::endl;

    r.set<Rect::Align::CENTER>(200, 200, 500, 500);
    std::cerr << r.toString() << std::endl;
    std::cerr << r.ToString() << std::endl;
}

int main(int argc, char *argv[])
{
    std::cerr << "Hello World" << std::endl;

    constructor_test();

    return 0;
}