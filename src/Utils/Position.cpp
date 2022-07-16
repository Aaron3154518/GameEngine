#include "Position.h"

bool eq(double v1, double v2, double err)
{
    return std::abs(v1 - v2) <= err;
}

// Rect
bool Rect::empty() const
{
    return eq(w, 0., ERR) && eq(h, 0., ERR);
}
bool Rect::invalid() const
{
    return w < 0 || h < 0;
}
void Rect::normalize()
{
    if (w < 0)
    {
        x += w;
        w = -w;
    }
    if (h < 0)
    {
        y += h;
        h = -h;
    }
}

// Getters
double Rect::x2() const
{
    return x + w;
}
double Rect::y2() const
{
    return y + h;
}
double Rect::cX() const
{
    return x + w / 2.0;
}
double Rect::cY() const
{
    return y + h / 2.0;
}

template <>
double Rect::getX<Rect::Align::TOP_LEFT>() const
{
    return x;
}
template <>
double Rect::getY<Rect::Align::TOP_LEFT>() const
{
    return y;
}

template <>
double Rect::getX<Rect::Align::BOT_RIGHT>() const
{
    return x2();
}
template <>
double Rect::getY<Rect::Align::BOT_RIGHT>() const
{
    return y2();
}

template <>
double Rect::getX<Rect::Align::CENTER>() const
{
    return cX();
}
template <>
double Rect::getY<Rect::Align::CENTER>() const
{
    return cY();
}

template <>
double Rect::getX<Rect::Align::CORNERS>() const
{
    return x;
}
template <>
double Rect::getY<Rect::Align::CORNERS>() const
{
    return y;
}

int Rect::X() const
{
    return (int)(x + ERR);
}
int Rect::Y() const
{
    return (int)(y + ERR);
}
int Rect::X2() const
{
    return (int)(x2() + ERR);
}
int Rect::Y2() const
{
    return (int)(y2() + ERR);
}
int Rect::W() const
{
    return X2() - X();
}
int Rect::H() const
{
    return Y2() - Y();
}
int Rect::CX() const
{
    return (int)(cX() + ERR);
}
int Rect::CY() const
{
    return (int)(cY() + ERR);
}

template <>
int Rect::GetX<Rect::Align::TOP_LEFT>() const
{
    return X();
}
template <>
int Rect::GetY<Rect::Align::TOP_LEFT>() const
{
    return Y();
}

template <>
int Rect::GetX<Rect::Align::BOT_RIGHT>() const
{
    return X2();
}
template <>
int Rect::GetY<Rect::Align::BOT_RIGHT>() const
{
    return Y2();
}

template <>
int Rect::GetX<Rect::Align::CENTER>() const
{
    return CX();
}
template <>
int Rect::GetY<Rect::Align::CENTER>() const
{
    return CY();
}

template <>
int Rect::GetX<Rect::Align::CORNERS>() const
{
    return X();
}
template <>
int Rect::GetY<Rect::Align::CORNERS>() const
{
    return Y();
}

Rect Rect::getMinRect(SDL_Texture *tex, double maxW, double maxH)
{
    int imgW, imgH;
    if (SDL_QueryTexture(tex, NULL, NULL, &imgW, &imgH) != 0)
    {
        SDL_Log("SDL_QueryTexture failed: %s", SDL_GetError());
        return Rect(0, 0, maxW, maxH);
    }
    Rect r = getMinRect(imgW, imgH, maxW, maxH);
#ifdef DEBUG
    std::cerr << "Got " << r << " from image with size " << imgW << " x " << imgH << std::endl;
#endif
    return r;
}
Rect Rect::getMinRect(double w, double h, double maxW, double maxH)
{
    // Save signs and make w/h positive
    int wSign = 1, hSign = 1;
    if (w < 0)
    {
        w *= -1;
        wSign = -1;
    }
    if (h < 0)
    {
        h *= -1;
        hSign = -1;
    }

    // maxW/maxH <= 0 or w/h <= 0 means ignore that dimensions
    bool noW = maxW <= 0 || w <= 0, noH = maxH <= 0 || h <= 0;
    double factor = 1.;
    if (!noW && !noH)
    {
        factor = std::fmin((double)maxW / w, (double)maxH / h);
    }
    else if (noW && !noH)
    {
        factor = (double)maxH / h;
    }
    else if (!noW && noH)
    {
        factor = (double)maxW / w;
    }
    return Rect(0, 0, wSign * w * factor, hSign * h * factor);
}

std::string Rect::toString() const
{
    std::stringstream ss;
    ss << "(" << x << ", " << y << ") -> ("
       << x2() << ", " << y2() << "), ("
       << w << "x" << h << "), ("
       << cX() << ", " << cY() << ")";
    return ss.str();
}
std::string Rect::ToString() const
{
    std::stringstream ss;
    ss << "(" << X() << ", " << Y() << ") -> ("
       << X2() << ", " << Y2() << "), ("
       << W() << "x" << H() << "), ("
       << CX() << ", " << CY() << ")";
    return ss.str();
}

// Setters

// Entire rect
template <>
void Rect::setX<Rect::Align::TOP_LEFT>(double v1, double v2)
{
    x = v1;
    w = v2;
}
template <>
void Rect::setY<Rect::Align::TOP_LEFT>(double v1, double v2)
{
    y = v1;
    h = v2;
}

template <>
void Rect::setX<Rect::Align::BOT_RIGHT>(double v1, double v2)
{
    x = v1 - v2;
    w = v2;
}
template <>
void Rect::setY<Rect::Align::BOT_RIGHT>(double v1, double v2)
{
    y = v1 - v2;
    h = v2;
}

template <>
void Rect::setX<Rect::Align::CENTER>(double v1, double v2)
{
    x = v1 - v2 / 2.0;
    w = v2;
}
template <>
void Rect::setY<Rect::Align::CENTER>(double v1, double v2)
{
    y = v1 - v2 / 2.0;
    h = v2;
}

template <>
void Rect::setX<Rect::Align::CORNERS>(double v1, double v2)
{
    x = v1;
    w = v2 - v1;
}
template <>
void Rect::setY<Rect::Align::CORNERS>(double v1, double v2)
{
    y = v1;
    h = v2 - v1;
}

// Rect position
template <>
void Rect::setPosX<Rect::Align::TOP_LEFT>(double _x)
{
    x = _x;
}
template <>
void Rect::setPosY<Rect::Align::TOP_LEFT>(double _y)
{
    y = _y;
}

template <>
void Rect::setPosX<Rect::Align::BOT_RIGHT>(double _x2)
{
    x = _x2 - w;
}
template <>
void Rect::setPosY<Rect::Align::BOT_RIGHT>(double _y2)
{
    y = _y2 - h;
}

template <>
void Rect::setPosX<Rect::Align::CENTER>(double _cx)
{
    x = _cx - w / 2.0;
}
template <>
void Rect::setPosY<Rect::Align::CENTER>(double _cy)
{
    y = _cy - h / 2.0;
}

template <>
void Rect::setPosX<Rect::Align::CORNERS>(double _x)
{
    x = _x;
}
template <>
void Rect::setPosY<Rect::Align::CORNERS>(double _y)
{
    y = _y;
}

// Operators
Rect::operator bool() const
{
    return !empty() && !invalid();
}
Rect::operator SDL_Rect() const
{
    return SDL_Rect{X(), Y(), W(), H()};
}

// Constructors
Rect::Rect() : Rect(0, 0, 0, 0) {}
// Use of setX<>(), setY<>() must happen AFTER specialization
Rect::Rect(double x, double y, double w, double h)
{
    setX<>(x, w);
    setY<>(y, h);
}
Rect::Rect(const SDL_Rect &other)
{
    set(other);
}