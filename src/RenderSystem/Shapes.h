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

// Shape rendering struct
struct ShapeData {
    virtual ~ShapeData() = default;

    SDL_Color color = BLACK;
    Rect boundary;
    SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;

    void copy(const ShapeData &data);

    Rect getBounds() const;

    virtual void draw() const;
};

// Rectangle
struct RectData : public ShapeData {
    RectData &set();
    RectData &set(const Rect &r);
    RectData &set(Rect r, int thickness, bool center = false);

    Rect r1, r2;

    void draw() const;
};

// Circle
struct CircleData : public ShapeData {
    CircleData &set(SDL_Point _c, int r);
    CircleData &set(SDL_Point _c, int r, int thickness, bool center = false);

    SDL_Point c{0, 0};
    int r1 = 0, r2 = 0;

    void draw() const;
};

struct ProgressBar : public ShapeData {
    ProgressBar &set(const Rect &r);
    ProgressBar &set(SDL_Color foreground, SDL_Color background);
    ProgressBar &set(float amnt, float cap, bool log = false);
    ProgressBar &set(float percent);

    Rect rect;
    float perc = 0.;
    SDL_Color bkgrnd = GRAY;

    void draw() const;
};

#endif