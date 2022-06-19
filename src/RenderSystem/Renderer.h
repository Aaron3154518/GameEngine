/*
This files contains a wrapper for the SDL_Renderer needed in all rendering.
The Renderer class cannot be instantiated and provides static functions for obtaining the renderer and creating textures.
*/

#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <stack>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Colors.h"
#include "../Utils/Rect/Rect.h"

// Renderer
class Renderer
{
	friend class TextureBuilder;
	friend void initRenderSystem(int w, int h, std::string);

public:
	Renderer() = delete;

	static SDL_Renderer *get();

private:
	// Renderer
	typedef std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer *)> RendererPtr;
	static RendererPtr mRenderer;

	// Renderer functions
	static void init(SDL_Window *window);
	static SDL_Point getTargetSize();
	static void getTargetSize(int *w, int *h);

	// Stack to track render targets
	static std::stack<SDL_Texture *> targetStack;
	// Adds to stack
	static void setRenderTarget(SDL_Texture *tex);
	// Changes top element of stack (reset() + set())
	static void changeRenderTarget(SDL_Texture *tex);
	// Pops top of stack
	static void resetRenderTarget();
	// Draw colors
	static std::stack<SDL_Color> colorStack;
	static void setDrawColor(const SDL_Color &c);
	static void changeDrawColor(const SDL_Color &c);
	static void resetDrawColor();
	// Blend modes
	static std::stack<SDL_BlendMode> blendStack;
	static void setBlendMode(SDL_BlendMode mode);
	static void changeBlendMode(SDL_BlendMode mode);
	static void resetBlendMode();
};

#endif