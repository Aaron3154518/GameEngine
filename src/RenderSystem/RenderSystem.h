/*
This files contains the entry point to this module
*/

#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <RenderSystem/AssetManager.h>
#include <RenderSystem/Renderer.h>
#include <RenderSystem/TextureBuilder.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include <exception>
#include <iostream>
#include <memory>

namespace RenderSystem
{

    class InitException : public std::exception
    {
        virtual const char *what() const throw()
        {
            return "Error attempting to initialize SDL systems";
        }
    };

    struct Options
    {
        // Dimensions
        int width = 0;
        int height = 0;
        bool maximize = false;

        // Window title
        std::string title = "My Game";

        // Asset manager parameters
        std::string defaultTexture = "";
    };

    typedef std::unique_ptr<SDL_Window, void (*)(SDL_Window *)> WindowPtr;
    void initRenderSystem(const Options &options);
    void teardownRenderSystem();

    void clearScreen(SDL_Color bkgrnd = BLACK);
    void presentScreen();

    void enforceFPS(int fps);

} // namespace RenderSystem

#endif