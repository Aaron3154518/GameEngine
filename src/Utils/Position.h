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
        setX<X>(v1x, v2x);
        setY<Y>(v1y, v2y);
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
        switch (A) {
            case Align::BOT_RIGHT:
                return x2();
            case Align::CENTER:
                return cX();
            default:
                return x;
        }
    }
    template <Align A = Align::TOP_LEFT>
    double getY() const {
        switch (A) {
            case Align::BOT_RIGHT:
                return y2();
            case Align::CENTER:
                return cY();
            default:
                return y;
        }
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
        switch (A) {
            case Align::BOT_RIGHT:
                return X2();
            case Align::CENTER:
                return CX();
            default:
                return X();
        }
    }
    template <Align A = Align::TOP_LEFT>
    int GetY() const {
        switch (A) {
            case Align::BOT_RIGHT:
                return Y2();
            case Align::CENTER:
                return CY();
            default:
                return Y();
        }
    }

    static Rect getMinRect(SDL_Texture* tex, double maxW, double maxH);
    static Rect getMinRect(double w, double h, double maxW, double maxH);

    // Setters
    // Set positions and dimensions
    template <Align X = Align::TOP_LEFT, Align Y = X>
    void set(const Rect& r) {
        switch (X) {
            case Align::CORNERS:
                setX<X>(r.getX<X>(), r.x2());
                break;
            default:
                setX<X>(r.getX<X>(), r.w);
                break;
        }

        switch (Y) {
            case Align::CORNERS:
                setY<Y>(r.getY<Y>(), r.y2());
                break;
            default:
                setY<Y>(r.getY<Y>(), r.h);
                break;
        }
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
