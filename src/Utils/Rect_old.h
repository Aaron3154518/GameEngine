#ifndef RECT_H
#define RECT_H

#include <SDL.h>
#include <SDL_image.h>

#include <cmath>
#include <iostream>

// namespace utils {
class Rect : public SDL_Rect {
   public:
    enum Align : uint8_t { TOP_LEFT = 0, CENTER, BOT_RIGHT };

    Rect();
    Rect(int x, int y, int w, int h);
    Rect(const SDL_Rect &other);
    ~Rect();

    Rect &operator=(const SDL_Rect &rhs);
    bool operator==(const Rect &rhs);
    bool operator!=(const Rect &rhs) { return !(*this == rhs); }

    int x2() const { return x + w; }
    int y2() const { return y + h; }
    float cX() const { return x + (float)w / 2; }
    float cY() const { return y + (float)h / 2; }
    SDL_Point topLeft() const { return SDL_Point{x, y}; }
    SDL_Point topRight() const { return SDL_Point{x + w, y}; }
    SDL_Point bottomLeft() const { return SDL_Point{x, y + h}; }
    SDL_Point bottomRight() const { return SDL_Point{x + w, y + h}; }
    SDL_Point center() const { return SDL_Point{x + w / 2, y + h / 2}; }
    SDL_Point dim() const { return SDL_Point{w, h}; }
    bool empty() const { return w == 0 || h == 0; }
    bool invalid() const { return w < 0 || h < 0; }

    void shift(int dX, int dY);
    void setPos(const Rect &r, Align mode);
    void setPos(const Rect &r, Align xMode, Align yMode);
    void setX2(float nX2) { x = (int)(nX2 - w); }
    void setY2(float nY2) { y = (int)(nY2 - h); }
    void setCX(float nCX) { x = (int)(nCX - w / 2); }
    void setCY(float nCY) { y = (int)(nCY - h / 2); }
    void setCenter(float nCX, float nCY);
    void setCenter(const SDL_Point &pos);
    void setTopLeft(const SDL_Point &pos);
    void setBottomRight(const SDL_Point &pos);
    void resize(SDL_Point nDim, bool center);
    void resize(int nW, int nH, bool center);
    void resizeFactor(float factor, bool center);
    void normalize();

    static Rect getMinRect(SDL_Texture *tex, int maxW, int maxH);
    static Rect getMinRect(int w, int h, int maxW, int maxH);

    operator bool() { return !empty() && !invalid(); }
    friend std::ostream &operator<<(std::ostream &os, const Rect &rhs);
    friend Rect &operator+=(Rect &lhs, const SDL_Point &rhs);
    friend Rect operator+(Rect lhs, const SDL_Point &rhs);
    friend Rect &operator-=(Rect &lhs, const SDL_Point &rhs);
    friend Rect operator-(Rect lhs, const SDL_Point &rhs);
};
//}

// TODO: Move these!
// For comparing SDL_Points
bool operator==(const SDL_Point &lhs, const SDL_Point &rhs);
bool operator!=(const SDL_Point &lhs, const SDL_Point &rhs);
// Point with point
SDL_Point &operator+=(SDL_Point &lhs, const SDL_Point &rhs);
SDL_Point operator+(SDL_Point lhs, const SDL_Point &rhs);
SDL_Point &operator-=(SDL_Point &lhs, const SDL_Point &rhs);
SDL_Point operator-(SDL_Point lhs, const SDL_Point &rhs);
SDL_Point &operator*=(SDL_Point &lhs, const SDL_Point &rhs);
SDL_Point operator*(SDL_Point lhs, const SDL_Point &rhs);
SDL_Point &operator/=(SDL_Point &lhs, const SDL_Point &rhs);
SDL_Point operator/(SDL_Point lhs, const SDL_Point &rhs);
// Point with float
SDL_Point &operator+=(SDL_Point &lhs, const float &rhs);
SDL_Point operator+(SDL_Point lhs, const float &rhs);
SDL_Point &operator-=(SDL_Point &lhs, const float &rhs);
SDL_Point operator-(SDL_Point lhs, const float &rhs);
SDL_Point &operator*=(SDL_Point &lhs, const float &rhs);
SDL_Point operator*(SDL_Point lhs, const float &rhs);
SDL_Point &operator/=(SDL_Point &lhs, const float &rhs);
SDL_Point operator/(SDL_Point lhs, const float &rhs);
std::ostream &operator<<(std::ostream &os, const SDL_Point &rhs);

#endif /* Rect */
