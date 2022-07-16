#define SDL_MAIN_HANDLED

#include <Utils/Position.h>

#include <iostream>

void set_test()
{
    std::cerr << "Testing Constructor" << std::endl;

    Rect r(0, 0, 500, 500);
    std::cerr << r.toString() << "\n"
              << r.ToString() << std::endl;

    std::cerr << std::endl;

    r.set<Rect::Align::CENTER>(200, 200, 500, 500);
    std::cerr << r.toString() << "\n"
              << r.ToString() << std::endl;

    std::cerr << std::endl;

    r.set<Rect::Align::BOT_RIGHT>(200, 200, 500, 500);
    std::cerr << r.toString() << "\n"
              << r.ToString() << std::endl;

    std::cerr << std::endl;

    r.set<Rect::Align::CORNERS>(200, 200, 500, 500);
    std::cerr << r.toString() << "\n"
              << r.ToString() << std::endl;
}

void normalize_test()
{
    std::cerr << "Testing Normalization" << std::endl;

    Rect r(300, 350, -200, -250);
    std::cerr << r.toString() << "\n"
              << r.ToString() << std::endl;

    std::cerr << std::endl;

    r.setPos<Rect::Align::TOP_LEFT, Rect::Align::CENTER>(50, 100);
    std::cerr << r.toString() << "\n"
              << r.ToString() << std::endl;

    std::cerr << std::endl;

    r.set<Rect::Align::CORNERS, Rect::Align::BOT_RIGHT>(100, 500, 0, -300);
    std::cerr << r.toString() << "\n"
              << r.ToString() << std::endl;
}

int main(int argc, char *argv[])
{
    std::cerr << "Hello World" << std::endl;

    set_test();
    std::cerr << "\n"
              << std::endl;
    normalize_test();

    return 0;
}