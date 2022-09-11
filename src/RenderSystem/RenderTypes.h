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

// To store a texture from tex, file, animation, etc.
class RenderTexture {
   public:
    RenderTexture(SharedTexture tex, unsigned int frameCnt = 1);
    RenderTexture(const std::string &file, unsigned int frameCnt = 1);
    RenderTexture(TextData &tData);
    RenderTexture(const AnimationData &animData);
    RenderTexture(TextDataWPtr tData);

    SharedTexture get() const;

    void setFrame(unsigned int frame);
    void nextFrame();
    unsigned int getNumFrames() const;
    unsigned int getFrame() const;

    SDL_Point getTextureDim() const;

    Uint32 getLastUpdated() const;

   private:
    void update();

    Uint32 mLastUpdated = 0;
    SDL_Point mDim;

    SharedTexture mTex;
    TextDataWPtr mTextSrc;

    unsigned int mFrameCnt = 1, mFrame = 0;
};

typedef std::shared_ptr<RenderTexture> RenderTexturePtr;
typedef std::shared_ptr<const RenderTexture> RenderTextureCPtr;

// To draw a texture
class RenderData : public Drawable {
   public:
    enum FitMode : uint8_t {
        Dest = 0,  // Uses dest rect
        Fit,       // Uses largest rect smaller than dest rect with image
                   // dimensions
        Texture,   // Uses image dimensions
    };

    using Drawable::Drawable;
    virtual ~RenderData() = default;

    void set(RenderTextureCPtr tex);
    void set(SharedTexture tex);
    void set(const std::string &file);
    void set(TextData &tData);
    void set(TextDataWPtr tData);

    void setDest(Rect r);
    void setArea(Rect r);
    void setAreaFrac(Rect r);
    void setBoundary(Rect r);
    void addBoundary(Rect r);

    void setRotationRad(float rotation);
    void setRotationDeg(float rotation);

    void setFit(FitMode fit);
    void setFitAlign(Rect::Align a = Rect::Align::CENTER);
    void setFitAlign(Rect::Align aX, Rect::Align aY);

    RenderTextureCPtr get() const;
    const Rect &getRect() const;
    Rect getDest() const;

    void draw(TextureBuilder &tex);

   private:
    RenderTextureCPtr mTex =
        std::make_shared<RenderTexture>(makeSharedTexture());

    Rect mRect, mBounds;
    Rect mArea = Rect(0, 0, 1, 1);

    float mRotation = 0;

    FitMode mFit = FitMode::Fit;
    Rect::Align mFitAlignX = Rect::Align::CENTER,
                mFitAlignY = Rect::Align::CENTER;
};

// To manage an animation
class RenderAnimation : public RenderData {
   public:
    RenderTexturePtr operator->() const;
    operator RenderTextureCPtr() const;

    void set(const AnimationData &data);

   private:
    RenderTexturePtr mAnim =
        std::make_shared<RenderTexture>(makeSharedTexture());
};

#endif