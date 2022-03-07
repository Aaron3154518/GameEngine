/*
This file contains typedefs for smart pointer implementations of various SDL pointer classes.
This file contains structs that hold data necessary for rendering text and textures.
*/

#ifndef RENDER_TYPES_H
#define RENDER_TYPES_H

#include <memory>
#include <sstream>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Renderer.h"
#include "Rect.h"

// Memory management for surfaces
typedef std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> Surface;
typedef std::shared_ptr<SDL_Surface> SharedSurface;
Surface makeSurface(SDL_Surface* surf = NULL);
SharedSurface makeSharedSurface(SDL_Surface* surf = NULL);

// Memory management for textures
typedef std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> Texture;
typedef std::shared_ptr<SDL_Texture> SharedTexture;
Texture makeTexture(SDL_Texture* tex = NULL);
SharedTexture makeSharedTexture(SDL_Texture* tex = NULL);

// Memory management for fonts
typedef std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> Font;
typedef std::shared_ptr<TTF_Font> SharedFont;
Font makeFont(TTF_Font* font = NULL);
SharedFont makeSharedFont(TTF_Font* font = NULL);

// Helper function to split text for wrapping
std::vector<std::string> splitText(const std::string& text,
	SharedFont font, int maxW);

// To render text
struct TextData {
	std::string text = "";
	SDL_Color color = BLACK;
	SDL_Color bkgrnd = TRANSPARENT;

	// For wrapping text
	// w > 0 will wrap text
	int w = 0;
	Rect::Align align = Rect::Align::CENTER;

	SharedFont font = makeSharedFont();

	// Functions to render text to a texture
	Texture renderText();
	Texture renderTextLine();
	Texture renderTextWrapped();
};

// To draw a texture
struct RenderData {
	SharedTexture texture;
	Rect dest, area, boundary;

	void fitToAsset();
	void fitToAsset(int maxW, int maxH);
};

// To draw a texture from text
struct TextRenderData : public RenderData {
	TextData tData;

	void renderText();
};

#endif