#define SDL_MAIN_HANDLED

#include <Utils/Rect.h>

#include <iostream>

void print(const Rect &r) {
    std::cerr << r << "\n"
              << r.ToString() << "\n"
              << r.toSDLRect() << std::endl;
}

void set_test() {
    std::cerr << "Testing Constructor" << std::endl;

    Rect r(0, 0, 500, 500);
    print(r);

    std::cerr << std::endl;

    r.set(200, 200, 500, 500, Rect::Align::CENTER);
    print(r);

    std::cerr << std::endl;

    r.set(200, 200, 500, 500, Rect::Align::BOT_RIGHT);
    print(r);

    std::cerr << std::endl;

    r.set(200, 200, 500, 500, Rect::Align::CORNERS);
    print(r);
}

void normalize_test() {
    std::cerr << "Testing Normalization" << std::endl;

    Rect r(300, 350, -200, -250);
    print(r);

    std::cerr << std::endl;

    r.setPos(50, 100, Rect::Align::TOP_LEFT, Rect::Align::CENTER);
    print(r);

    std::cerr << std::endl;

    r.set(100, 500, 0, -300, Rect::Align::CORNERS, Rect::Align::BOT_RIGHT);
    print(r);
}

void modify_test() {
    std::cerr << "Testing Modification" << std::endl;

    Rect r(0, 0, 500, 500);
    print(r);

    std::cerr << std::endl;

    r.move(50, -50);
    print(r);

    std::cerr << std::endl;

    r.move(50, -50, 100);
    print(r);

    std::cerr << std::endl;

    r.moveFactor(.5, .2, Rect::Align::CENTER, Rect::Align::TOP_LEFT);
    print(r);

    std::cerr << std::endl;

    r.setDim(300, 200, Rect::Align::CENTER, Rect::Align::BOT_RIGHT);
    print(r);

    std::cerr << std::endl;

    r.resize(2.5, Rect::Align::TOP_LEFT, Rect::Align::CENTER);
    print(r);

    std::cerr << std::endl;

    r.fitWithin(Rect(150, 150, 1000, 1000));
    print(r);
}

int main(int argc, char *argv[]) {
    std::cerr << "Hello World" << std::endl;

    set_test();
    std::cerr << "\n" << std::endl;
    normalize_test();
    std::cerr << "\n" << std::endl;
    modify_test();

    return 0;
}