/*
This file contains the TextureBuilder class used to draw to a texture or the
screen. It contains various drawing functions for shapes in Shapes.h as well as
other visual modifications.
*/

#ifndef TEXTURE_BUILDER_H
#define TEXTURE_BUILDER_H

#include <RenderSystem/AssetManager.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <Utils/Colors.h>
#include <Utils/Rect.h>

// Forward declaration
class TextureBuilder;

struct Drawable {
    Drawable(SDL_Color color = BLACK, SDL_BlendMode mode = SDL_BLENDMODE_NONE);
    Drawable(SDL_BlendMode mode, SDL_Color color = BLACK);
    virtual ~Drawable() = default;

    SDL_BlendMode mBlendMode = SDL_BLENDMODE_NONE;
    SDL_Color mColor = BLACK;

    virtual void draw(TextureBuilder &tex);
};

class TextureBuilder {
   public:
    TextureBuilder() = default;
    TextureBuilder(int w, int h, SDL_Color bkgrnd = TRANSPARENT);
    TextureBuilder(const std::string &src);
    TextureBuilder(SharedTexture src, bool copy = true);
    ~TextureBuilder() = default;

    // Get texture
    SharedTexture getTexture();

    // Start new texture
    void reset(int w, int h, SDL_Color bkgrnd = TRANSPARENT);

    // Draw textures/text
    void draw(Drawable &drawable);

    // Brighten texture
    void brighten(Uint8 strength);

    SDL_Point getTextureSize();
    static SDL_Point getTextureSize(SDL_Texture *tex);

   private:
    SharedTexture mTex = makeSharedTexture();

    void endDrawShape();
};

#endif