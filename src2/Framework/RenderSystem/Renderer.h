/*
This files contains a wrapper for the SDL_Renderer needed in all rendering.
The Renderer class cannot be instantiated and provides static functions for
obtaining the renderer and creating textures.
*/

#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <Utils/Colors.h>
#include <Utils/Pointers.h>
#include <Utils/Rect.h>

#include <iostream>
#include <memory>
#include <stack>
#include <vector>

// Forward declaration
namespace RenderSystem {
struct Options;
void init(const Options &options);
}  // namespace RenderSystem

// Renderer
class Renderer {
    friend void RenderSystem::init(const Options &options);

   public:
    class NullRendererException : public std::exception {
        virtual const char *what() const throw() {
            return "Error: SDL_Renderer was null while attempting get() with "
                   "notNull = true. Did you forget to initialize?";
        }
    };

    Renderer() = delete;

    static SDL_Renderer *get(bool notNull = true);

    static SDL_Point getTargetSize();
    static void getTargetSize(int *w, int *h);

    static void setRenderTarget(SDL_Texture *tex);
    static void setDrawColor(const SDL_Color &c);
    static void setBlendMode(SDL_BlendMode mode);

   private:
    // Renderer
    static RendererPtr mRenderer;

    // Renderer functions
    static void init(SDL_Window *window);
};

#endif