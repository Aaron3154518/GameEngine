/*
This files contains the entry point to this module
*/

#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <exception>
#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>

#include "AssetManager.h"
#include "Renderer.h"
#include "TextureBuilder.h"

class InitException : public std::exception {
	virtual const char* what() const throw() {
		return "Error attempting to initialize SDL systems";
	}
};

typedef std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> WindowPtr;
void initRenderSystem(int w, int h, std::string name = "Game");
void teardownRenderSystem();

void clearScreen(SDL_Color bkgrnd = BLACK);
void presentScreen();

void enforceFPS(int fps);

#endif