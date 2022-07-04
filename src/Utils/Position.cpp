#include "Position.h"

// Constructors
Rect::Rect() : Rect(0, 0, 0, 0) {}

template <>
Rect::Rect<Rect::Align::TOP_LEFT>(double _x, double _y, double _w, double _h)
    : x(_x), y(_y), w(_w), h(_h) {}

template <>
Rect::Rect<Rect::Align::BOT_RIGHT>(double _x2, double _y2, double _w, double _h)
    : x(_x2 - _w), y(_y2 - _h), w(_w), h(_h) {}

template <>
Rect::Rect<Rect::Align::CENTER>(double _cx, double _cy, double _w, double _h)
    : x(_cx - _w / 2.0), y(_cy - _h / 2.0), w(_w), h(_h) {}

template <>
Rect::Rect<Rect::Align::CORNERS>(double _x1, double _y1, double _x2, double _y2)
    : x(_x1), y(_y1), w(_x2 - _x1), h(_y2 - _y1) {}

Rect::Rect(const SDL_Rect& other)
    : x(other.x), y(other.y), w(other.w), h(other.h) {}

// Getters
int Rect::X() const {
    return (int)(x + ERR);
}
int Rect::Y() const {
    return (int)(y + ERR);
}
int Rect::X2() const {
    return (int)(x + ERR + w + ERR);
}
int Rect::Y2() const {
    return (int)(y + ERR + h + ERR);
}
int Rect::W() const {
    return X2() - X();
}
int Rect::H() const {
    return Y2() - Y();
}
int Rect::CX() const {
    return (int)((w + ERR) / 2);
}
int Rect::CY() const {
    return (int)((h + ERR) / 2);
}

// Operators
Rect::operator SDL_Rect() {
    return SDL_Rect{X(), Y(), W(), H()};
}