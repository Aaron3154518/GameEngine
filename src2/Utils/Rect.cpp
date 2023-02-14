#include "Rect.h"

bool eq(float v1, float v2, float err) { return std::abs(v1 - v2) <= err; }

std::ostream &operator<<(std::ostream &os, const Dimensions &d) {
    os << d.w << "x" << d.h;
    return os;
}

std::ostream &operator<<(std::ostream &os, const DimensionsF &d) {
    os << d.w << "x" << d.h;
    return os;
}

// Constructors
Rect::Rect() : Rect(0, 0, 0, 0) {}
Rect::Rect(float x, float y, float w, float h) : _x(x), _y(y), _w(w), _h(h) {
    normalize();
}
Rect::Rect(const SDL_Rect &other) : Rect(other.x, other.y, other.w, other.h) {}

// Rect
bool Rect::empty() const { return eq(_w, 0., ERR) || eq(_h, 0., ERR); }
bool Rect::Empty() const { return W() == 0 || H() == 0; }
bool Rect::invalid() const { return _w < 0 || _h < 0; }
bool Rect::Invalid() const { return W() < 0 || H() < 0; }
void Rect::normalize() {
    if (_w < 0) {
        _x += _w;
        _w = -_w;
    }
    if (_h < 0) {
        _y += _h;
        _h = -_h;
    }
    SDL_Rect::x = X();
    SDL_Rect::y = Y();
    SDL_Rect::w = W();
    SDL_Rect::h = H();
}
void Rect::move(float dX, float dY) {
    setPos(_x + dX, _y + dY, Align::TOP_LEFT);
}
void Rect::move(float mag, float dX, float dY) {
    float dMag = sqrt(dX * dX + dY * dY);
    float factor = dMag == 0 ? 0 : mag / dMag;
    setPos(_x + dX * factor, _y + dY * factor, Align::TOP_LEFT);
}
void Rect::moveFactor(float factorX, float factorY, Align a) {
    moveFactor(factorX, factorY, a, a);
}
void Rect::moveFactor(float factorX, float factorY, Align aX, Align aY) {
    setPosX(getX(aX) * factorX, aX);
    setPosY(getY(aY) * factorY, aY);
}
void Rect::resize(float factor, Align a) {
    setDim(_w * factor, _h * factor, a, a);
}
void Rect::resize(float factor, Align aX, Align aY) {
    setDim(_w * factor, _h * factor, aX, aY);
}
void Rect::fitWithin(const Rect &boundary) {
    setPosX(std::max(std::min(_x, boundary.x2() - _w), boundary.x()));
    setPosY(std::max(std::min(_y, boundary.y2() - _h), boundary.y()));
}

// Getters
float Rect::x() const { return _x; }
float Rect::y() const { return _y; }
float Rect::x2() const { return _x + _w; }
float Rect::y2() const { return _y + _h; }
float Rect::w() const { return _w; }
float Rect::h() const { return _h; }
float Rect::cX() const { return _x + _w / 2.0; }
float Rect::cY() const { return _y + _h / 2.0; }
float Rect::halfW() const { return _w / 2.0; }
float Rect::halfH() const { return _h / 2.0; }
float Rect::minDim() const { return fminf(w(), h()); }
float Rect::maxDim() const { return fmaxf(w(), h()); }

float Rect::getX(Align a) const {
    switch (a) {
        case Align::TOP_LEFT:
        case Align::CORNERS:
            return _x;
        case Align::BOT_RIGHT:
            return x2();
        case Align::CENTER:
            return cX();
    }

    throw std::runtime_error("Rect::getX(): Invalid Align value");
}
float Rect::getY(Align a) const {
    switch (a) {
        case Align::TOP_LEFT:
        case Align::CORNERS:
            return _y;
        case Align::BOT_RIGHT:
            return y2();
        case Align::CENTER:
            return cY();
    }

    throw std::runtime_error("Rect::getY(): Invalid Align value");
}
SDL_FPoint Rect::getPos(Align a) const { return {getX(a), getY(a)}; }
SDL_FPoint Rect::getPos(Align aX, Align aY) const {
    return {getX(aX), getY(aY)};
}

int Rect::X() const { return (int)(_x + copysign(ERR, _x)); }
int Rect::Y() const { return (int)(_y + copysign(ERR, _y)); }
int Rect::X2() const {
    float _x2 = x2();
    return (int)(_x2 + copysign(ERR, _x2));
}
int Rect::Y2() const {
    float _y2 = y2();
    return (int)(_y2 + copysign(ERR, _y2));
}
int Rect::W() const { return X2() - X(); }
int Rect::H() const { return Y2() - Y(); }
int Rect::CX() const {
    float _cX = cX();
    return (int)(_cX + copysign(ERR, _cX));
}
int Rect::CY() const {
    float _cY = cY();
    return (int)(_cY + copysign(ERR, _cY));
}
int Rect::MinDim() const { return std::min(W(), H()); }
int Rect::MaxDim() const { return std::max(W(), H()); }

int Rect::GetX(Align a) const {
    switch (a) {
        case Align::TOP_LEFT:
        case Align::CORNERS:
            return X();
        case Align::BOT_RIGHT:
            return X2();
        case Align::CENTER:
            return CX();
    }

    throw std::runtime_error("Rect::GetX(): Invalid Align value");
}
int Rect::GetY(Align a) const {
    switch (a) {
        case Align::TOP_LEFT:
        case Align::CORNERS:
            return Y();
        case Align::BOT_RIGHT:
            return Y2();
        case Align::CENTER:
            return CY();
    }

    throw std::runtime_error("Rect::GetY(): Invalid Align value");
}
SDL_Point Rect::GetPos(Align a) const { return {GetX(a), GetY(a)}; }
SDL_Point Rect::GetPos(Align aX, Align aY) const {
    return {GetX(aX), GetY(aY)};
}

Rect Rect::getMinRect(SDL_Texture *tex, float maxW, float maxH) {
    int imgW, imgH;
    if (SDL_QueryTexture(tex, NULL, NULL, &imgW, &imgH) != 0) {
        SDL_Log("SDL_QueryTexture failed: %s", SDL_GetError());
        return Rect(0, 0, maxW, maxH);
    }
    Rect r = getMinRect(imgW, imgH, maxW, maxH);
#ifdef DEBUG
    std::cerr << "Got " << r << " from image with size " << imgW << " x "
              << imgH << " and max dimensions " << maxW << " x " << maxH
              << std::endl;
#endif
    return r;
}
Rect Rect::getMinRect(float w, float h, float maxW, float maxH) {
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
    float factor = 1.;
    if (!noW && !noH) {
        factor = std::fmin((float)maxW / w, (float)maxH / h);
    } else if (noW && !noH) {
        factor = (float)maxH / h;
    } else if (!noW && noH) {
        factor = (float)maxW / w;
    }
    return Rect(0, 0, wSign * w * factor, hSign * h * factor);
}
Rect Rect::fitToTexture(SDL_Texture *tex) {
    int imgW, imgH;
    if (SDL_QueryTexture(tex, NULL, NULL, &imgW, &imgH) != 0) {
        SDL_Log("SDL_QueryTexture failed: %s", SDL_GetError());
        return Rect(0, 0, 0, 0);
    }
    Rect r(0, 0, imgW, imgH);
#ifdef DEBUG
    std::cerr << "Got " << r << " from image with size " << imgW << " _x "
              << imgH << std::endl;
#endif
    return r;
}

std::string Rect::toString() const {
    std::stringstream ss;
    ss << "(" << _x << ", " << _y << ") -> (" << x2() << ", " << y2() << "), ("
       << _w << "x" << _h << "), (" << cX() << ", " << cY() << ")";
    return ss.str();
}
std::string Rect::ToString() const {
    std::stringstream ss;
    ss << "(" << X() << ", " << Y() << ") -> (" << X2() << ", " << Y2()
       << "), (" << W() << "x" << H() << "), (" << CX() << ", " << CY() << ")";
    return ss.str();
}

SDL_Rect Rect::toSDLRect() const { return {X(), Y(), W(), H()}; }

// Setters
// Entire rect
void Rect::set(const Rect &r, Align a) { set(r, a, a); }
void Rect::set(const Rect &r, Align aX, Align aY) {
    set(r.getX(aX), r.getY(aY), aX == Align::CORNERS ? r.x2() : r._w,
        aY == Align::CORNERS ? r.y2() : r._h, aX, aY);
}
void Rect::set(float v1x, float v1y, float v2x, float v2y, Align a) {
    set(v1x, v1y, v2x, v2y, a, a);
}
void Rect::set(float v1x, float v1y, float v2x, float v2y, Align aX, Align aY) {
    setPosX(v1x, aX);
    if (aX == Align::CORNERS) {
        setWidth(v2x - v1x, Align::TOP_LEFT);
    } else {
        setWidth(v2x, aX);
    }
    setPosY(v1y, aY);
    if (aY == Align::CORNERS) {
        setHeight(v2y - v1y, Align::TOP_LEFT);
    } else {
        setHeight(v2y, aY);
    }
}

void Rect::setPos(const Rect &r, Align a) { setPos(r, a, a); }
void Rect::setPos(const Rect &r, Align aX, Align aY) {
    setPosX(r.getX(aX), aX);
    setPosY(r.getY(aY), aY);
}
void Rect::setPos(float vx, float vy, Align a) { setPos(vx, vy, a, a); }
void Rect::setPos(float vx, float vy, Align aX, Align aY) {
    setPosX(vx, aX);
    setPosY(vy, aY);
}

void Rect::setDim(float nW, float nH, Align a) { setDim(nW, nH, a, a); }
void Rect::setDim(float nW, float nH, Align aX, Align aY) {
    setWidth(nW, aX);
    setHeight(nH, aY);
}

void Rect::setPosX(float vx, Align a) {
    switch (a) {
        case Align::TOP_LEFT:
        case Align::CORNERS:
            _x = vx;
            break;
        case Align::BOT_RIGHT:
            _x = vx - _w;
            break;
        case Align::CENTER:
            _x = vx - _w / 2.0;
            break;
    }
    normalize();
}
void Rect::setPosY(float vy, Align a) {
    switch (a) {
        case Align::TOP_LEFT:
        case Align::CORNERS:
            _y = vy;
            break;
        case Align::BOT_RIGHT:
            _y = vy - _h;
            break;
        case Align::CENTER:
            _y = vy - _h / 2.0;
            break;
    }
    normalize();
}

void Rect::setWidth(float vW, Align a) {
    switch (a) {
        case Align::TOP_LEFT:
        case Align::CORNERS:
            break;
        case Align::BOT_RIGHT:
            _x += _w - vW;
            break;
        case Align::CENTER:
            _x += (_w - vW) / 2.0;
            break;
    }
    _w = vW;
    normalize();
}
void Rect::setHeight(float vH, Align a) {
    switch (a) {
        case Align::TOP_LEFT:
        case Align::CORNERS:
            break;
        case Align::BOT_RIGHT:
            _y += _h - vH;
            break;
        case Align::CENTER:
            _y += (_h - vH) / 2.0;
            break;
    }
    _h = vH;
    normalize();
}

// Operators
Rect::operator bool() const { return !empty() && !invalid(); }
Rect::operator const SDL_Rect *() const { return (SDL_Rect *)this; }

std::ostream &operator<<(std::ostream &os, const Rect &rhs) {
    os << rhs.toString();
    return os;
}
std::ostream &operator<<(std::ostream &os, const SDL_Rect &rhs) {
    os << "(" << rhs.x << ", " << rhs.y << ", " << rhs.w << ", " << rhs.h
       << ")";
    return os;
}

bool Rect::operator==(const Rect &rhs) const {
    return eq(_x, rhs._x, ERR) && eq(_y, rhs._y, ERR) && eq(_w, rhs._w, ERR) &&
           eq(_h, rhs._h, ERR);
}
bool Rect::operator!=(const Rect &rhs) const { return !(*this == rhs); }
