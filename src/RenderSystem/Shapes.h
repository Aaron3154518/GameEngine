/*
This file contains structs for drawing shapes and other visual items.
Each struct contains data for rendering the item and functions to set the data.
*/

#ifndef SHAPES_H
#define SHAPES_H

#include <RenderSystem/Renderer.h>
#include <SDL.h>
#include <Utils/Colors.h>
#include <Utils/Rect.h>

#include <cmath>
#include <iostream>

constexpr float TWO_PI = M_PI + M_PI;
constexpr float HALF_PI = M_PI / 2;
constexpr float DEG_TO_RAD = M_PI / 180;

// Forward declaration
class TextureBuilder;

// Shape rendering struct
struct ShapeData {
    virtual ~ShapeData() = default;

    SDL_Color color = BLACK;
    Rect boundary;
    SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;

    void copy(const ShapeData &data);

    Rect getBounds() const;

    virtual void draw(TextureBuilder &tex) const;
};

// Rectangle
struct RectData : public ShapeData {
    RectData &set();
    RectData &set(const Rect &r);
    RectData &set(Rect r, int thickness, bool center = false);

    Rect r1, r2;

    void draw(TextureBuilder &tex) const;
};

// Circle
struct CircleData : public ShapeData {
    CircleData &set(SDL_Point _c, int r);
    CircleData &set(SDL_Point _c, int r, int thickness, bool center = false);
    CircleData &setFullCircle();
    CircleData &setAngleRad(float _a1, float _a2);
    CircleData &setAngleDeg(float _a1, float _a2);
    CircleData &setDashed(unsigned int d);

    SDL_Point c{0, 0};
    int r1 = 0, r2 = 0;
    float a1 = 0, a2 = TWO_PI;
    unsigned int dashes = 0;

    struct Sector {
        float a1, a2;
        int quad;
    };

    void draw(TextureBuilder &tex) const;

   private:
    void drawCircle(const Rect &bounds) const;
    void drawSectors(const Rect &bounds) const;
    void drawSector(const Sector &s, const Rect &bounds) const;
    void drawDashed(const Rect &bounds) const;
};

struct ProgressBar : public ShapeData {
    ProgressBar &set(const Rect &r);
    ProgressBar &set(SDL_Color foreground, SDL_Color background);
    ProgressBar &set(float amnt, float cap, bool log = false);
    ProgressBar &set(float percent);

    Rect rect;
    float perc = 0.;
    SDL_Color bkgrnd = GRAY;

    void draw(TextureBuilder &tex) const;
};

#endif