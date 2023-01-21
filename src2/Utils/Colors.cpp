#include "Colors.h"

// Colors
bool operator==(const SDL_Color &lhs, const SDL_Color &rhs) {
    return lhs.a == rhs.a && lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}
bool operator!=(const SDL_Color &lhs, const SDL_Color &rhs) {
    return !(lhs == rhs);
}
std::ostream &operator<<(std::ostream &os, const SDL_Color &rhs) {
    os << "(" << (int)rhs.r << ", " << (int)rhs.g << ", " << (int)rhs.b << ", "
       << (int)rhs.a << ")";
    return os;
}

Uint32 toUint(SDL_Color c) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint8 arr[4] = {c.r, c.g, c.b, c.a};
    return *(Uint32 *)arr;
#else
    Uint8 arr[4] = {c.a, c.b, c.g, c.r};
    return *(Uint32 *)arr;
#endif
}