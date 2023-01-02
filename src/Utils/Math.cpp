#include "Math.h"

namespace Math {
float max_mag(float v1, float v2) { return fabsf(v1) >= fabsf(v2) ? v1 : v2; }
float min_mag(float v1, float v2) { return fabsf(v1) <= fabsf(v2) ? v1 : v2; }
}  // namespace Math
