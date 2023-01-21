/*
This files contains the AssetManager class which manages all fonts and loaded
images.
*/

#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <Framework/RenderSystem/Renderer.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <Utils/Pointers.h>
#include <Utils/Rect.h>
#include <sys/stat.h>

#include <iostream>
#include <map>
#include <string>

// Data for creating a font
struct FontData {
    int w = -1, h = -1;
    std::string sample = "";
    std::string file = "res/fonts/times.ttf";

    bool operator<(const FontData &rhs) const;
};

class AssetManager {
   public:
    AssetManager() = delete;
    ~AssetManager() = delete;

    static SharedTexture getTexture(std::string file);
    static SharedFont getFont(const FontData &data);

    static const std::string &getDefaultTexture();
    static void setDefaultTexture(const std::string &str);

    static Dimensions getTextureSize(SDL_Texture *tex);
    static Dimensions getFontSize(std::string fileName, int size);
    static Dimensions getTextSize(std::string fileName, int size,
                                  std::string sampleText);

   private:
    static std::string mDefaultTexture;
    static std::map<std::string, SharedTexture> mTextures;
    static std::map<FontData, SharedFont> mFonts;
};

#endif