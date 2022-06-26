/*
This file contains the TextureBuilder class used to draw to a texture or the screen.
It contains various drawing functions for shapes in Shapes.h as well as other visual modifications.
*/

#ifndef TEXTURE_BUILDER_H
#define TEXTURE_BUILDER_H

#include <SDL.h>
#include <SDL_ttf.h>

#include "../Utils/Colors.h"
#include "../Utils/Rect.h"
#include "RenderTypes.h"
#include "Shapes.h"

class TextureBuilder {
   public:
    TextureBuilder() = default;
    TextureBuilder(int w, int h, SDL_Color bkgrnd = TRANSPARENT);
    TextureBuilder(SharedTexture src);
    ~TextureBuilder() = default;

    // Get texture
    SharedTexture getTexture();

    // Start new texture
    void reset(int w, int h, SDL_Color bkgrnd = TRANSPARENT);

    // Draw textures/text
    void draw(const RenderData &data);
    void draw(const RectData &data);
    void draw(const CircleData &data);
    void draw(const ProgressBar &data);

    // Brighten texture
    void brighten(Uint8 strength);

    static bool getTextureSize(SDL_Texture *tex, int *w, int *h);

   private:
    SharedTexture mTex = makeSharedTexture();

    void startDrawShape(const ShapeData &data);
    Rect getShapeBounds(const ShapeData &data);
    void endDrawShape();
};

#endif