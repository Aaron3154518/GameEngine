#ifndef MATH_H
#define MATH_H

#include <SDL.h>

#include <cmath>

namespace Math {
// Takes max/min value based on magnitude
float max_mag(float v1, float v2);
float min_mag(float v1, float v2);

bool bitsSet(uint8_t val, uint8_t check);
float distance(SDL_Point p1, SDL_Point p2);

float round(float val, float precision);
}  // namespace Math

#endif
