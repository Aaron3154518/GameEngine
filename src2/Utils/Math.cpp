#include "Math.h"

namespace Math {
float max_mag(float v1, float v2) { return fabsf(v1) >= fabsf(v2) ? v1 : v2; }
float min_mag(float v1, float v2) { return fabsf(v1) <= fabsf(v2) ? v1 : v2; }

bool allBitsSet(uint8_t val, uint8_t check) { return (val & check) == check; }
bool anyBitsSet(uint8_t val, uint8_t check) { return (val & check) != 0; }

float distance(SDL_Point p1, SDL_Point p2) {
    float dx = p1.x - p2.x, dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

float round(float val, float precision) {
    return floor(val / precision + 0.5) * precision;
}
}  // namespace Math
