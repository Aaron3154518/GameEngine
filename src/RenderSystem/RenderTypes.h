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
#include <Utils/AnimationData.h>
#include <Utils/Rect.h>

#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Forward Declaration
class TextData;

typedef std::weak_ptr<TextData> TextDataWPtr;

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
    RenderData &set(TextDataWPtr tData);
    RenderData &set(TextData &tData);
    RenderData &set(const AnimationData &animData);

    RenderData &setDest(Rect r);
    RenderData &setArea(Rect r);
    RenderData &setBoundary(Rect r);
    RenderData &addBoundary(Rect r);

    RenderData &setRotationRad(float rotation);
    RenderData &setRotationDeg(float rotation);

    RenderData &setFit(FitMode fit);
    RenderData &setFitAlign(Rect::Align a = Rect::Align::CENTER);
    RenderData &setFitAlign(Rect::Align aX, Rect::Align aY);

    RenderData &setFrame(unsigned int frame);
    RenderData &nextFrame();

    const Rect &getRect() const;
    const Rect &getDest() const;

    Uint32 getLastUpdated() const;

    unsigned int getNumFrames() const;
    unsigned int getFrame() const;

    void draw(TextureBuilder &tex);

   private:
    RenderData &update();

    Uint32 mLastUpdated = 0;

    SharedTexture mTex;
    TextDataWPtr mTextSrc;

    SDL_Point mDim{0, 0};
    Rect mRect, mDest, mBounds, mArea;

    float mRotation = 0;

    FitMode mFit = FitMode::Fit;
    Rect::Align mFitAlignX = Rect::Align::CENTER,
                mFitAlignY = Rect::Align::CENTER;

    unsigned int mFrameCnt = 1, mFrame = 0;
};

typedef std::shared_ptr<RenderData> RenderDataPtr;
typedef std::weak_ptr<RenderData> RenderDataWPtr;

#endif