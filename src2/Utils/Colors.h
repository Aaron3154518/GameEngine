/*
This file contains color constants and functions related to colors.
*/

#ifndef COLORS_H
#define COLORS_H

#include <SDL.h>

#include <iostream>

namespace Colors {
// ARGB masks for creating surfaces and colors
constexpr Uint32 rmask = 0xff000000;
constexpr Uint32 gmask = 0x00ff0000;
constexpr Uint32 bmask = 0x0000ff00;
constexpr Uint32 amask = 0x000000ff;

// Colors
constexpr SDL_Color Transparent{0, 0, 0, 0};
constexpr SDL_Color White{255, 255, 255, 255};
constexpr SDL_Color LGray{175, 175, 175, 255};
constexpr SDL_Color Gray{128, 128, 128, 255};
constexpr SDL_Color Black{0, 0, 0, 255};
constexpr SDL_Color Red{255, 0, 0, 255};
constexpr SDL_Color Orange{255, 165, 0, 255};
constexpr SDL_Color Yellow{255, 255, 0, 255};
constexpr SDL_Color Green{0, 255, 0, 255};
constexpr SDL_Color Cyan{0, 255, 255, 255};
constexpr SDL_Color Blue{0, 0, 255, 255};
constexpr SDL_Color Magenta{255, 0, 255, 255};
constexpr SDL_Color Purple{128, 0, 128, 255};
}  // namespace Colors

bool operator==(const SDL_Color &lhs, const SDL_Color &rhs);
bool operator!=(const SDL_Color &lhs, const SDL_Color &rhs);
std::ostream &operator<<(std::ostream &os, const SDL_Color &rhs);

Uint32 toUint(SDL_Color c);

#endif