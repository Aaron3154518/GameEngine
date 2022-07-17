#ifndef POSITION_H
#define POSITION_H

#include <SDL.h>

#include <cmath>
#include <sstream>
#include <string>

bool eq(float v1, float v2, float err);

class Rect : private SDL_Rect {
   private:
    float _x, _y, _w, _h;

   public:
    enum Align : uint8_t { TOP_LEFT = 0, CENTER, BOT_RIGHT, CORNERS };

    float ERR = 1e-5;

    Rect();
    Rect(float x, float y, float w, float h);
    Rect(const SDL_Rect &other);
    ~Rect() = default;

    bool empty() const;
    bool invalid() const;
    void normalize();
    void move(float dX, float dY);
    void move(float mag, float dX, float dY);
    void resize(float factor, Align aX = Align::TOP_LEFT);
    void resize(float factor, Align aX, Align aY);

    // Getters
    float x() const;
    float y() const;
    float x2() const;
    float y2() const;
    float w() const;
    float h() const;
    float cX() const;
    float cY() const;

    float getX(Align a = Align::TOP_LEFT) const;
    float getY(Align a = Align::TOP_LEFT) const;

    int X() const;
    int Y() const;
    int X2() const;
    int Y2() const;
    int W() const;
    int H() const;
    int CX() const;
    int CY() const;

    float GetX(Align a = Align::TOP_LEFT) const;
    float GetY(Align a = Align::TOP_LEFT) const;

    static Rect getMinRect(SDL_Texture *tex, float maxW, float maxH);
    static Rect getMinRect(float w, float h, float maxW, float maxH);

    std::string toString() const;
    std::string ToString() const;

    // Setters
    // Set positions and dimensionsd
    void set(const Rect &r, Align aX = Align::TOP_LEFT);
    void set(const Rect &r, Align aX, Align aY);
    void set(float v1x, float v1y, float v2x, float v2y,
             Align aX = Align::TOP_LEFT);
    void set(float v1x, float v1y, float v2x, float v2y, Align aX, Align aY);

    void setPos(const Rect &r, Align aX = Align::TOP_LEFT);
    void setPos(const Rect &r, Align aX, Align aY);
    void setPos(float vx, float vy, Align aX = Align::TOP_LEFT);
    void setPos(float vx, float vy, Align aX, Align aY);

    void setDim(float nW, float nH, Align aX = Align::TOP_LEFT);
    void setDim(float nW, float nH, Align aX, Align aY);

    void setPosX(float val, Align a = Align::TOP_LEFT);
    void setPosY(float val, Align a = Align::TOP_LEFT);

    void setWidth(float vW, Align a = Align::TOP_LEFT);
    void setHeight(float vH, Align a = Align::TOP_LEFT);

    // Operators
    operator bool() const;
    operator SDL_Rect *() const;
    friend std::ostream &operator<<(std::ostream &os, const Rect &rhs);
};

std::ostream &operator<<(std::ostream &os, const SDL_Rect &rhs);

#endif
