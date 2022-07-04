#include "Position.h"

bool eq(double v1, double v2, double err) {
    return std::abs(v1 - v2) <= err;
}

// Rect
// Constructors
Rect::Rect() : Rect(0, 0, 0, 0) {}
Rect::Rect(const SDL_Rect& other) {
    set<>(other);
}

bool Rect::empty() const {
    return eq(w, 0., ERR) && eq(h, 0., ERR);
}
bool Rect::invalid() const {
    return w < 0 || h < 0;
}
void Rect::normalize() {
    if (w < 0) {
        x += w;
        w = -w;
    }
    if (h < 0) {
        y += h;
        h = -h;
    }
}

// Getters
double Rect::x2() const {
    return x + w;
}
double Rect::y2() const {
    return y + h;
}
double Rect::cX() const {
    return x + w / 2.0;
}
double Rect::cY() const {
    return y + h / 2.0;
}

int Rect::X() const {
    return (int)(x + ERR);
}
int Rect::Y() const {
    return (int)(y + ERR);
}
int Rect::X2() const {
    return (int)(x2() + ERR);
}
int Rect::Y2() const {
    return (int)(y2() + ERR);
}
int Rect::W() const {
    return X2() - X();
}
int Rect::H() const {
    return Y2() - Y();
}
int Rect::CX() const {
    return (int)(cX() + ERR);
}
int Rect::CY() const {
    return (int)(cY() + ERR);
}

Rect Rect::getMinRect(SDL_Texture* tex, double maxW, double maxH) {
    int imgW, imgH;
    if (SDL_QueryTexture(tex, NULL, NULL, &imgW, &imgH) != 0) {
        SDL_Log("SDL_QueryTexture failed: %s", SDL_GetError());
        return Rect(0, 0, maxW, maxH);
    }
    Rect r = getMinRect(imgW, imgH, maxW, maxH);
#ifdef DEBUG
    std::cerr << "Got " << r << " from image with size " << imgW << " x " << imgH << std::endl;
#endif
    return r;
}
Rect Rect::getMinRect(double w, double h, double maxW, double maxH) {
    // Save signs and make w/h positive
    int wSign = 1, hSign = 1;
    if (w < 0) {
        w *= -1;
        wSign = -1;
    }
    if (h < 0) {
        h *= -1;
        hSign = -1;
    }

    // maxW/maxH <= 0 or w/h <= 0 means ignore that dimensions
    bool noW = maxW <= 0 || w <= 0, noH = maxH <= 0 || h <= 0;
    double factor = 1.;
    if (!noW && !noH) {
        factor = std::fmin((double)maxW / w, (double)maxH / h);
    } else if (noW && !noH) {
        factor = (double)maxH / h;
    } else if (!noW && noH) {
        factor = (double)maxW / w;
    }
    return Rect(0, 0, wSign * w * factor, hSign * h * factor);
}

// Setters
// Position and dimensions
template <>
void Rect::setX<Rect::Align::TOP_LEFT>(double _x, double _w) {
    x = _x;
    w = _w;
}
template <>
void Rect::setY<Rect::Align::TOP_LEFT>(double _y, double _h) {
    y = _y;
    h = _h;
}

template <>
void Rect::setX<Rect::Align::BOT_RIGHT>(double _x2, double _w) {
    x = _x2 - _w;
    w = _w;
}
template <>
void Rect::setY<Rect::Align::BOT_RIGHT>(double _y2, double _h) {
    y = _y2 - _h;
    h = _h;
}

template <>
void Rect::setX<Rect::Align::CENTER>(double _cx, double _w) {
    x = _cx - _w / 2.0;
    w = _w;
}
template <>
void Rect::setY<Rect::Align::CENTER>(double _cy, double _h) {
    y = _cy - _h / 2.0;
    h = _h;
}

template <>
void Rect::setX<Rect::Align::CORNERS>(double _x1, double _x2) {
    x = _x1;
    w = _x2 - _x1;
}
template <>
void Rect::setY<Rect::Align::CORNERS>(double _y1, double _y2) {
    y = _y1;
    h = _y2 - _y1;
}

// Position only
template <>
void Rect::setPosX<Rect::Align::TOP_LEFT>(double _x) {
    x = _x;
}
template <>
void Rect::setPosY<Rect::Align::TOP_LEFT>(double _y) {
    y = _y;
}

template <>
void Rect::setPosX<Rect::Align::BOT_RIGHT>(double _x2) {
    x = _x2 - w;
}
template <>
void Rect::setPosY<Rect::Align::BOT_RIGHT>(double _y2) {
    y = _y2 - h;
}

template <>
void Rect::setPosX<Rect::Align::CENTER>(double _cx) {
    x = _cx - w / 2.0;
}
template <>
void Rect::setPosY<Rect::Align::CENTER>(double _cy) {
    y = _cy - h / 2.0;
}

template <>
void Rect::setPosX<Rect::Align::CORNERS>(double _x) {
    x = _x;
}
template <>
void Rect::setPosY<Rect::Align::CORNERS>(double _y) {
    y = _y;
}

// Operators
Rect::operator bool() const {
    return !empty() && !invalid();
}
Rect::operator SDL_Rect() const {
    return SDL_Rect{X(), Y(), W(), H()};
}
