/*
This file contains structs for drawing shapes and other visual items.
Each struct contains data for rendering the item and functions to set the data.
*/

#ifndef SHAPES_H
#define SHAPES_H

#include <RenderSystem/AssetManager.h>
#include <RenderSystem/Renderer.h>
#include <RenderSystem/TextureBuilder.h>
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
struct Shape : public Drawable {
    using Drawable::Drawable;

    Rect boundary;

    virtual ~Shape() = default;

    Rect getBounds() const;
};

// Rectangle
struct RectData {
    Rect r1, r2;
};

struct RectShape : public Shape {
    using Shape::Shape;

    const RectData &get() const;

    RectShape &set(const Rect &r = Rect(0, 0, 0, 0));
    RectShape &set(Rect r, int thickness, bool center = false);

    void draw(TextureBuilder &tex);

   private:
    RectData data;
};

// Circle
struct CircleData {
    SDL_Point c{0, 0};
    int r1 = 0, r2 = 0;
    float a1 = 0, a2 = TWO_PI;
    unsigned int dashes = 0;
};

struct CircleShape : public Shape {
    using Shape::Shape;

    const CircleData &get() const;

    CircleShape &setCenter(SDL_Point c);
    CircleShape &setRadius(int r);
    CircleShape &setRadius(int r, int thickness, bool center = false);
    CircleShape &setFullCircle();
    CircleShape &setAngleRad(float a1, float a2);
    CircleShape &setAngleDeg(float a1, float a2);
    CircleShape &setDashed(unsigned int dashes);

    struct Sector {
        float a1, a2;
        int quad;
    };

    void draw(TextureBuilder &tex);

   private:
    CircleData data;

    void drawCircle(const Rect &bounds) const;
    void drawSectors(const Rect &bounds) const;
    void drawSector(const Sector &s, const Rect &bounds) const;
    void drawDashed(const Rect &bounds) const;
};

// Progress Base
struct ProgressBarData {
    float perc = 0.;
    Rect fullRect, emptyRect;
};

struct ProgressBar : public Shape {
    using Shape::Shape;

    Rect dest;

    const ProgressBarData &get() const;

    ProgressBar &set(const Rect &r);
    ProgressBar &set(SDL_Color foreground, SDL_Color background);
    ProgressBar &set(float amnt, float cap, bool log = false);
    ProgressBar &set(float percent);

    void draw(TextureBuilder &tex);

   private:
    SDL_Color mBkgrnd = GRAY;

    ProgressBarData data;
};

#endif