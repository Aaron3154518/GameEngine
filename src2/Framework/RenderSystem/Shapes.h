/*
This file contains structs for drawing shapes and other visual items.
Each struct contains data for rendering the item and functions to set the data.
*/

#ifndef SHAPES_H
#define SHAPES_H

#include <Framework/RenderSystem/AssetManager.h>
#include <Framework/RenderSystem/Renderer.h>
#include <Framework/RenderSystem/TextureBuilder.h>
#include <SDL.h>
#include <Utils/Colors.h>
#include <Utils/Rect.h>

#include <cmath>
#include <iostream>

constexpr float TWO_PI = M_PI + M_PI;
constexpr float HALF_PI = M_PI / 2;
constexpr float DEG_TO_RAD = M_PI / 180;

namespace Shapes {
// Shape Attributes
struct ShapeColor {
   public:
    virtual ~ShapeColor() = default;

    void setColor(SDL_Color c);

   protected:
    SDL_Color color = Colors::Black;
};

struct ShapeBlendmode {
   public:
    virtual ~ShapeBlendmode() = default;

    void setBlendMode(SDL_BlendMode b);

   protected:
    SDL_BlendMode blend = SDL_BLENDMODE_NONE;
};

struct ShapeBoundary {
   public:
    virtual ~ShapeBoundary() = default;

    void setBoundary(const Rect &b);

   protected:
    Rect getBounds();

    Rect bounds;
};

// Rectangle
struct Rectangle : public Drawable,
                   public ShapeColor,
                   public ShapeBlendmode,
                   public ShapeBoundary {
    struct Data {
        Rect r1, r2;
    };

    const Data &operator()() const;

    void set(const Rect &r = Rect(0, 0, 0, 0));
    void set(const Rect &r1, const Rect &r2);
    void set(Rect r, int thickness, bool center = false);

    void draw(TextureBuilder &tex);

   private:
    Data data;
};

// Circle
struct Circle : public Drawable,
                public ShapeColor,
                public ShapeBlendmode,
                public ShapeBoundary {
    struct Data {
        SDL_Point c{0, 0};
        int r1 = 0, r2 = 0;
        float a1 = 0, a2 = TWO_PI;
        unsigned int dashes = 0;
    };

    struct Sector {
        float a1, a2;
        int quad;
    };

    const Data &operator()() const;

    void setCenter(SDL_Point c);
    void setRadius(int r);
    void setRadius(int r, int thickness, bool center = false);
    void setFullCircle();
    void setAngleRad(float a1, float a2);
    void setAngleDeg(float a1, float a2);
    void setDashed(unsigned int dashes);

    void draw(TextureBuilder &tex);

   private:
    Data data;

    void drawCircle(const Rect &bounds) const;
    void drawSectors(const Rect &bounds) const;
    void drawSector(const Sector &s, const Rect &bounds) const;
    void drawDashed(const Rect &bounds) const;
};

// ProgressBar
struct ProgressBar : public Drawable,
                     public ShapeBlendmode,
                     public ShapeBoundary {
    struct Data {
        float perc = 0.;
        Rect rect;
        SDL_Color fgrnd, bkgrnd;
    };

    const Data &operator()() const;

    void set(const Rect &r);
    void set(SDL_Color foreground, SDL_Color background);
    void set(float amnt, float cap, bool log = false);
    void set(float percent);

    void draw(TextureBuilder &tex);

    Rect getFullRect() const;
    Rect getEmptyRect() const;

   private:
    Data data;
};
}  // namespace Shapes

#endif