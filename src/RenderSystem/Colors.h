/*
This file contains color constants and functions related to colors.
*/

#ifndef COLORS_H
#define COLORS_H

#include <SDL.h>

// ARGB masks for creating surfaces and colors
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
constexpr Uint32 rmask = 0xff000000;
constexpr Uint32 gmask = 0x00ff0000;
constexpr Uint32 bmask = 0x0000ff00;
constexpr Uint32 amask = 0x000000ff;
#else
constexpr Uint32 rmask = 0x000000ff;
constexpr Uint32 gmask = 0x0000ff00;
constexpr Uint32 bmask = 0x00ff0000;
constexpr Uint32 amask = 0xff000000;
#endif

// Colors
constexpr SDL_Color TRANSPARENT{ 0,0,0,0 };
constexpr SDL_Color WHITE{ 255,255,255,255 };
constexpr SDL_Color LGRAY{ 175,175,175,255 };
constexpr SDL_Color GRAY{ 128,128,128,255 };
constexpr SDL_Color BLACK{ 0,0,0,255 };
constexpr SDL_Color RED{ 255,0,0,255 };
constexpr SDL_Color ORANGE{ 255,165,0,255 };
constexpr SDL_Color YELLOW{ 255,255,0,255 };
constexpr SDL_Color GREEN{ 0,255,0,255 };
constexpr SDL_Color CYAN{ 0,255,255,255 };
constexpr SDL_Color BLUE{ 0,0,255,255 };
constexpr SDL_Color MAGENTA{ 255,0,255,255 };
constexpr SDL_Color PURPLE{ 128,0,128,255 };

bool operator==(const SDL_Color& lhs, const SDL_Color& rhs);
bool operator!=(const SDL_Color& lhs, const SDL_Color& rhs);

Uint32 toUint(SDL_Color c);

#endif