#ifndef POINTERS_H
#define POINTERS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <memory>

// Surfaces
typedef std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> Surface;
typedef std::shared_ptr<SDL_Surface> SharedSurface;
Surface makeSurface(SDL_Surface *surf = NULL);
SharedSurface makeSharedSurface(SDL_Surface *surf = NULL);

// Textures
typedef std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> Texture;
typedef std::shared_ptr<SDL_Texture> SharedTexture;
Texture makeTexture(SDL_Texture *tex = NULL);
SharedTexture makeSharedTexture(SDL_Texture *tex = NULL);

// Fonts
typedef std::unique_ptr<TTF_Font, void (*)(TTF_Font *)> Font;
typedef std::shared_ptr<TTF_Font> SharedFont;
Font makeFont(TTF_Font *font = NULL);
SharedFont makeSharedFont(TTF_Font *font = NULL);

// Windows/renderers
typedef std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> WindowPtr;
typedef std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> RendererPtr;

#endif
