#include "RenderTypes.h"

#include <RenderSystem/TextRender.h>

// RenderTexture
RenderTexture::RenderTexture(SharedTexture tex, unsigned int frameCnt) {
    mTex = tex;
    mFrameCnt = frameCnt;
    mFrame = 0;
    if (!AssetManager::getTextureSize(mTex.get(), &mDim.x, &mDim.y)) {
        mDim = {0, 0};
    }
    if (fmod(mDim.x, mFrameCnt) != 0) {
        throw std::runtime_error(
            "RenderData::set(): " + std::to_string(mFrameCnt) +
            " does not evenly divide sprite sheet with length " +
            std::to_string(mDim.x));
    }
    mDim.x /= mFrameCnt;
    update();
}
RenderTexture::RenderTexture(const std::string &file, unsigned int frameCnt)
    : RenderTexture(AssetManager::getTexture(file), frameCnt) {}
RenderTexture::RenderTexture(TextData &tData) : RenderTexture(tData.get()) {}
RenderTexture::RenderTexture(const AnimationData &animData)
    : RenderTexture(animData.file, animData.num_frames) {}
RenderTexture::RenderTexture(TextDataWPtr tData) : mTextSrc(tData) {
    auto textSrc = mTextSrc.lock();
}

SharedTexture RenderTexture::get() const {
    auto ptr = mTextSrc.lock();
    if (ptr) {
        return ptr->get();
    }

    return mTex;
}

void RenderTexture::setFrame(unsigned int frame) {
    mFrame = frame % mFrameCnt;
    update();
}
void RenderTexture::nextFrame() {
    setFrame(mFrame + 1);
    update();
}
unsigned int RenderTexture::getNumFrames() const { return mFrameCnt; }
unsigned int RenderTexture::getFrame() const { return mFrame; }

SDL_Point RenderTexture::getTextureDim() const {
    auto ptr = mTextSrc.lock();
    if (ptr) {
        return TextureBuilder::getTextureSize(ptr->get().get());
    }

    return mDim;
}

void RenderTexture::update() { mLastUpdated = SDL_GetTicks(); }
Uint32 RenderTexture::getLastUpdated() const { return mLastUpdated; }

// RenderData
void RenderData::set(RenderTextureCPtr tex) { mTex = tex; }
void RenderData::set(SharedTexture tex) {
    set(std::make_shared<RenderTexture>(tex));
}
void RenderData::set(const std::string &file) {
    set(std::make_shared<RenderTexture>(file));
}
void RenderData::set(TextData &tData) {
    set(std::make_shared<RenderTexture>(tData));
}
void RenderData::set(TextDataWPtr tData) {
    set(std::make_shared<RenderTexture>(tData));
}

void RenderData::setDest(Rect r) { mRect = r; }
void RenderData::setArea(Rect r) {
    SDL_Point texDim = mTex->getTextureDim();

    if (texDim.x == 0 || texDim.y == 0) {
        mArea = Rect();
        return;
    }

    SDL_Rect result;
    Rect tmp(0, 0, texDim.x, texDim.y);
    SDL_IntersectRect(r, tmp, &result);
    tmp = result;
    mArea = Rect(tmp.x() / texDim.x, tmp.y() / texDim.y, tmp.w() / texDim.x,
                 tmp.h() / texDim.y);
}
void RenderData::setAreaFrac(Rect r) {
    mArea = Rect(fminf(r.x(), 1), fminf(r.y(), 1), fminf(r.w(), 1),
                 fminf(r.h(), 1));
}
void RenderData::setBoundary(Rect r) { mBounds = r; }
void RenderData::addBoundary(Rect r) {
    SDL_Rect result;
    SDL_IntersectRect(mBounds, r, &result);
    mBounds = result;
}

void RenderData::setRotationRad(float rotation) {
    mRotation = rotation / DEG_TO_RAD;
}
void RenderData::setRotationDeg(float rotation) { mRotation = rotation; }

void RenderData::setFit(FitMode fit) { mFit = fit; }
void RenderData::setFitAlign(Rect::Align a) { setFitAlign(a, a); }
void RenderData::setFitAlign(Rect::Align aX, Rect::Align aY) {
    mFitAlignX = aX;
    mFitAlignY = aY;
}

RenderTextureCPtr RenderData::get() const { return mTex; }

RenderTextureCPtr RenderData::operator->() const { return mTex; }

RenderData::operator RenderTextureCPtr() const { return mTex; }

const Rect &RenderData::getRect() const { return mRect; }
Rect RenderData::getDest() const {
    SDL_Point texDim = mTex->getTextureDim();

    Rect dest;
    switch (mFit) {
        case FitMode::Dest:
            dest = mRect;
            break;
        case FitMode::Fit:
            dest = Rect::getMinRect(texDim.x, texDim.y, mRect.w(), mRect.h());
            dest.setPos(mRect, mFitAlignX, mFitAlignY);
            break;
        case FitMode::Texture:
            dest = Rect(0, 0, texDim.x, texDim.y);
            dest.setPos(mRect, mFitAlignX, mFitAlignY);
            break;
    };
    return dest;
}

float RenderData::getRotationRad() const { return mRotation * DEG_TO_RAD; }
float RenderData::getRotationDeg() const { return mRotation; }

void RenderData::draw(TextureBuilder &tex) {
    // Check the texture to draw
    if (!mTex || !mTex->get()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Invalid Texture" << std::endl;
#endif
        return;
    }

    SDL_Point texDim = mTex->getTextureDim();

    Rect dest = getDest();

    Rect area(mArea.x() * texDim.x, mArea.y() * texDim.y, mArea.w() * texDim.x,
              mArea.h() * texDim.y);

    // Make sure we are actually drawing something
    if (dest.empty() || area.empty()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Empty destination rect" << std::endl;
#endif
        return;
    }

    int w, h;
    Renderer::getTargetSize(&w, &h);
    Rect renderBounds(0, 0, w, h);

    // Get the boundary rect
    Rect boundary = mBounds;
    SDL_Rect result;
    if (boundary.empty()) {
        boundary = renderBounds;
    } else {
        if (SDL_IntersectRect(boundary, renderBounds, &result) == SDL_FALSE) {
#ifdef RENDER_DEBUG
            std::cerr << "draw(): Boundary rect " << boundary
                      << " was outside the screen: " << renderBounds
                      << std::endl;
#endif
            return;
        }
        boundary = result;
    }

    // Get fraction of item to draw
    float leftFrac = fmaxf(boundary.x() - dest.x(), 0) / dest.w();
    float topFrac = fmaxf(boundary.y() - dest.y(), 0) / dest.h();
    float rightFrac = fmaxf(dest.x2() - boundary.x2(), 0) / dest.w();
    float botFrac = fmaxf(dest.y2() - boundary.y2(), 0) / dest.h();

    dest = Rect(dest.x() + dest.w() * leftFrac, dest.y() + dest.h() * topFrac,
                dest.w() * (1. - leftFrac - rightFrac),
                dest.h() * (1. - topFrac - botFrac));

    // Make sure the rect is actually in the boundary
    if (leftFrac + rightFrac >= 1 || topFrac + botFrac >= 1) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Rect " << dest << " was out side the boundary "
                  << boundary << std::endl;
#endif
        return;
    }

    area = Rect(area.x() + area.w() * leftFrac + texDim.x * mTex->getFrame(),
                area.y() + area.h() * topFrac,
                area.w() * (1.f - leftFrac - rightFrac),
                area.h() * (1.f - topFrac - botFrac));

    // Make sure at least one pixel will be drawn
    if (area.empty()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Can't draw from " << area << " to " << dest
                  << std::endl;
#endif
        return;
    }

    SDL_RenderCopyEx(Renderer::get(), mTex->get().get(), area, dest, mRotation,
                     NULL, SDL_FLIP_NONE);
}

// RenderAnimation
RenderTexturePtr RenderAnimation::operator->() const { return mAnim; }

RenderAnimation::operator RenderTextureCPtr() const { return mAnim; }

void RenderAnimation::set(const AnimationData &data) {
    mAnim = std::make_shared<RenderTexture>(data);
    RenderData::set(mAnim);
}