#include "Position.h"

bool eq(double v1, double v2, double err) {
    return std::abs(v1 - v2) <= err;
}

// Rect
// Constructors
Rect::Rect() : Rect(0, 0, 0, 0) {}
Rect::Rect(double x, double y, double w, double h) {
    setX<>(x, w);
    setY<>(y, h);
}
Rect::Rect(const SDL_Rect& other) {
    set(other);
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

std::string Rect::toString() const {
    std::stringstream ss;
    ss << "(" << x << ", " << y << ") -> ("
       << x2() << ", " << y2() << "), ("
       << w << "x" << h << "), ("
       << cX() << ", " << cY() << ")";
    return ss.str();
}
std::string Rect::ToString() const {
    std::stringstream ss;
    ss << "(" << X() << ", " << Y() << ") -> ("
       << X2() << ", " << Y2() << "), ("
       << W() << "x" << H() << "), ("
       << CX() << ", " << CY() << ")";
    return ss.str();
}

// Operators
Rect::operator bool() const {
    return !empty() && !invalid();
}
Rect::operator SDL_Rect() const {
    return SDL_Rect{X(), Y(), W(), H()};
}
