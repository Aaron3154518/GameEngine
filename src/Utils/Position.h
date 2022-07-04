#ifndef POSITION_H
#define POSITION_H

#include <SDL.h>

#include <cmath>

bool eq(double v1, double v2, double err);

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
    template <Align X = Align::TOP_LEFT, Align Y = X>
    Rect(double v1x, double v1y, double v2x, double v2y) {
        set<X>(v1x, v2x);
        set<Y>(v1y, v2y);
    }
    Rect(const SDL_Rect& other);
    ~Rect() = default;

    bool empty() const;
    bool invalid() const;
    void normalize();
    template <Align X = Align::TOP_LEFT, Align Y = X>
    void resize(double nW, double nH) {
        double tmpX = getX<X>(), tmpY = getY<Y>();
        w = nW;
        h = nH;
        setPos<X, Y>(tmpX, tmpY);
    }
    template <Align X = Align::TOP_LEFT, Align Y = X>
    void resize(double factor) {
        resize<X, Y>(w * factor, h * factor);
    }

    // Getters
    double x2() const;
    double y2() const;
    double cX() const;
    double cY() const;

    template <Align A = Align::TOP_LEFT>
    double getX() const {
        return
#if A == Align::BOT_RIGHT
            x2();
#elif A == Align::CENTER
            cX();
#else
            x;
#endif
    }
    template <Align A = Align::TOP_LEFT>
    double getY() const {
        return
#if A == Align::BOT_RIGHT
            y2();
#elif A == Align::CENTER
            cY();
#else
            y;
#endif
    }

    int X() const;
    int Y() const;
    int X2() const;
    int Y2() const;
    int W() const;
    int H() const;
    int CX() const;
    int CY() const;

    template <Align A = Align::TOP_LEFT>
    int GetX() const {
        return
#if A == Align::BOT_RIGHT
            X2();
#elif A == Align::CENTER
            CX();
#else
            X();
#endif
    }
    template <Align A = Align::TOP_LEFT>
    int GetY() const {
        return
#if A == Align::BOT_RIGHT
            Y2();
#elif A == Align::CENTER
            CY();
#else
            Y();
#endif
    }

    static Rect getMinRect(SDL_Texture* tex, double maxW, double maxH);
    static Rect getMinRect(double w, double h, double maxW, double maxH);

    // Setters
    // Set positions and dimensions
    template <Align X = Align::TOP_LEFT, Align Y = X>
    void set(const Rect& r) {
        setX<X>(r.getX<X>(),
#if X == Align::CORNERS
                r.x2()
#else
                r.w
#endif
        );

        setY<Y>(r.getY<Y>(),
#if Y == Align::CORNERS
                r.y2()
#else
                r.h
#endif
        );
    }
    template <Align X = Align::TOP_LEFT, Align Y = X>
    void set(double v1x, double v1y, double v2x, double v2y) {
        setX<X>(v1x, v2x);
        setY<Y>(v1y, v2y);
    }

    template <Align A = Align::TOP_LEFT>
    void setX(double v1, double v2);
    template <Align A = Align::TOP_LEFT>
    void setY(double v1, double v2);

    // Set only position
    template <Align X = Align::TOP_LEFT, Align Y = X>
    void setPos(const Rect& r) {
        setPosX<X>(r.getX<X>());
        setPosY<Y>(r.getY<Y>());
    }
    template <Align X = Align::TOP_LEFT, Align Y = X>
    void setPos(double vx, double vy) {
        setPosX<X>(vx);
        setPosY<Y>(vy);
    }

    template <Align A = Align::TOP_LEFT>
    void setPosX(double val);
    template <Align A = Align::TOP_LEFT>
    void setPosY(double val);

    // Operators
    operator bool() const;
    operator SDL_Rect() const;

   private:
};

#endif
