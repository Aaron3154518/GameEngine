/*
This file contains typedefs for smart pointer implementations of various SDL
pointer classes. This file contains structs that hold data necessary for
rendering text and textures.
*/

#ifndef RENDER_TYPES_H
#define RENDER_TYPES_H

#include <RenderSystem/AssetManager.h>
#include <RenderSystem/Renderer.h>
#include <RenderSystem/TextureBuilder.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <Utils/Rect.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Helper function to split text for wrapping
std::vector<std::string> splitText(const std::string &text, SharedFont font,
                                   int maxW);

// To render text
struct TextData {
    std::string text = "";
    SDL_Color color = BLACK;
    SDL_Color bkgrnd = TRANSPARENT;

    // For wrapping text
    // w > 0 will wrap text
    int w = 0;
    bool autoFit = true;
    Rect::Align align = Rect::Align::CENTER;

    SharedFont font = makeSharedFont();

    // Functions to render text to a texture
    Texture renderText() const;
    Texture renderTextLine() const;
    Texture renderTextWrapped() const;
};

// To draw a texture
class RenderData : public Drawable {
    friend class AnimationData;

   public:
    enum FitMode : uint8_t {
        Dest = 0,  // Uses dest rect
        Fit,       // Uses largest rect smaller than dest rect with image
                   // dimensions
        Texture,   // Uses image dimensions
    };

    using Drawable::Drawable;

    RenderData &set(SharedTexture tex, unsigned int frameCnt = 1);
    RenderData &set(const std::string &file, unsigned int frameCnt = 1);
    RenderData &set(const TextData &tData, unsigned int frameCnt = 1);

    RenderData &setDest(Rect r);
    RenderData &setArea(Rect r);
    RenderData &setBoundary(Rect r);
    RenderData &addBoundary(Rect r);

    RenderData &setFit(FitMode fit);
    RenderData &setFitAlign(Rect::Align a = Rect::Align::CENTER);
    RenderData &setFitAlign(Rect::Align aX, Rect::Align aY);

    const Rect &getRect() const;
    const Rect &getDest() const;

    void nextFrame();

    void draw(TextureBuilder &tex) const;

   private:
    SharedTexture mTex;
    SDL_Point mDim;
    unsigned int mFrameCnt = 1, mFrame = 0;
    FitMode mFit = FitMode::Fit;
    Rect::Align mFitAlignX = Rect::Align::CENTER,
                mFitAlignY = Rect::Align::CENTER;
    Rect mRect, mDest, mBounds, mArea;
};

#endif