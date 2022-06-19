/*
This files contains the AssetManager class which manages all fonts and loaded images.
*/

#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <map>
#include <string>
#include <sys/stat.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Renderer.h"
#include "RenderTypes.h"

// Data for creating a font
struct FontData
{
	int w = -1, h = -1;
	std::string sample = "";
	std::string file = "res/fonts/times.ttf";

	bool operator<(const FontData &rhs) const;
};

class AssetManager
{
public:
	AssetManager() = default;
	~AssetManager() = default;

	SharedTexture getTexture(std::string file);
	SharedFont getFont(const FontData &data);

	static bool getTextureSize(SDL_Texture *tex, int *w, int *h);
	static void getFontSize(std::string fileName, int size, int *w, int *h);
	static void getTextSize(std::string fileName, int size,
							std::string sampleText, int *w, int *h);

private:
	std::map<std::string, SharedTexture> mTextures;
	std::map<FontData, SharedFont> mFonts;
};

#endif