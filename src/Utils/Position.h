#ifndef POSITION_H
#define POSITION_H

#include <SDL.h>

class Rect {
   public:
    enum Align : uint8_t {
        TOP_LEFT = 0,
        CENTER,
        BOT_RIGHT,
        CORNERS
    };

    double x, y, w, h;
    double ERR = 1e-5;

    Rect();
    template <Align A = Align::TOP_LEFT>
    Rect(double v1, double v2, double v3, double v4) {}
    Rect(const SDL_Rect &other);
    ~Rect() = default;

    int X() const;
    int Y() const;
    int X2() const;
    int Y2() const;
    int W() const;
    int H() const;
    int CX() const;
    int CY() const;

    operator SDL_Rect();

   private:
};

#endif
