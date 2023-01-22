#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <Framework/RenderSystem/AssetManager.h>
#include <Framework/RenderSystem/Renderer.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <Utils/Colors.h>
#include <Utils/Pointers.h>

#include <exception>
#include <iostream>
#include <memory>

namespace RenderSystem {
class InitException : public std::exception {
    virtual const char *what() const throw() {
        return "Error attempting to initialize SDL systems";
    }
};

struct Options {
    // Dimensions
    int width = 0;
    int height = 0;
    bool maximize = false;

    // Window title
    std::string title = "My Game";

    // Asset manager parameters
    std::string defaultTexture = "";
};

void init(const Options &options);
void clean();

void clearScreen(SDL_Color bkgrnd = Colors::Black);
void presentScreen();

void enforceFPS(uint32_t fps);

SDL_Point getWindowSize();

}  // namespace RenderSystem

#endif