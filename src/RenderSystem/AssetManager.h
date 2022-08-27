/*
This files contains the AssetManager class which manages all fonts and loaded
images.
*/

#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <RenderSystem/Renderer.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sys/stat.h>

#include <iostream>
#include <map>
#include <string>

// Memory management for surfaces
typedef std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)> Surface;
typedef std::shared_ptr<SDL_Surface> SharedSurface;
Surface makeSurface(SDL_Surface *surf = NULL);
SharedSurface makeSharedSurface(SDL_Surface *surf = NULL);

// Memory management for textures
typedef std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)> Texture;
typedef std::shared_ptr<SDL_Texture> SharedTexture;
Texture makeTexture(SDL_Texture *tex = NULL);
SharedTexture makeSharedTexture(SDL_Texture *tex = NULL);

// Memory management for fonts
typedef std::unique_ptr<TTF_Font, void (*)(TTF_Font *)> Font;
typedef std::shared_ptr<TTF_Font> SharedFont;
Font makeFont(TTF_Font *font = NULL);
SharedFont makeSharedFont(TTF_Font *font = NULL);

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

    static bool getTextureSize(SDL_Texture *tex, int *w, int *h);
    static void getFontSize(std::string fileName, int size, int *w, int *h);
    static void getTextSize(std::string fileName, int size,
                            std::string sampleText, int *w, int *h);

   private:
    static std::string mDefaultTexture;
    static std::map<std::string, SharedTexture> mTextures;
    static std::map<FontData, SharedFont> mFonts;
};

#endif