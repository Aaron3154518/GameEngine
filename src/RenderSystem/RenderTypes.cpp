#include "RenderTypes.h"

// TextData
// Functions to render text to a texture
SharedTexture TextData::renderText() const {
    if (w <= 0) {
        return renderTextLine();
    } else {
        return renderTextWrapped();
    }
}
SharedTexture TextData::renderTextLine() const {
    SharedTexture tex = makeSharedTexture();
    if (text.empty()) {
        return tex;
    }
    if (!font) {
        std::cerr << "renderText(): Invalid font" << std::endl;
        return tex;
    }
    Surface surface = makeSurface();
    if (bkgrnd == TRANSPARENT) {
        surface.reset(TTF_RenderText_Blended(font.get(), text.c_str(), color));
    } else {
        surface.reset(
            TTF_RenderText_Shaded(font.get(), text.c_str(), color, bkgrnd));
    }
    if (!surface) {
#ifdef DEBUG_RENDER
        std::cerr << "renderText(): Unable to render text" << std::endl;
#endif
        return tex;
    }
    tex = makeSharedTexture(
        SDL_CreateTextureFromSurface(Renderer::get(), surface.get()));
    return tex;
}
SharedTexture TextData::renderTextWrapped() const {
    if (!font) {
        std::cerr << "renderTextWrapped(): Could not load font" << std::endl;
        return makeSharedTexture();
    }
    std::string textCopy = text;
    std::unique_ptr<std::list<LinePtr>> lines = splitText(textCopy, font, w);
    if (lines->empty()) {
        return makeSharedTexture();
    }

    int maxLineW = 0;
    if (autoFit) {
        for (auto &line : *lines) {
            if (line->mW > maxLineW) {
                maxLineW = line->mW;
            }
        }
    } else {
        maxLineW = w;
    }

    int lineH = TTF_FontHeight(font.get());
    int h = lineH * lines->size();
    TextureBuilder tex(maxLineW, h, bkgrnd);
    Rect lineR(0, 0, maxLineW, lineH);
    for (auto &line : *lines) {
        if (!line->empty()) {
            line->draw(tex, lineR, align, font.get(), textCopy, color);
        }
        lineR.move(0, lineH);
    }
    return tex.getTexture();
}

// RenderData
RenderData &RenderData::set(SharedTexture tex, unsigned int frameCnt) {
    mTex = tex;
    mFrameCnt = frameCnt;
    mFrame = 0;
    if (!AssetManager::getTextureSize(mTex.get(), &mDim.x, &mDim.y)) {
        std::cerr << "RenderData::setTexture(): Failed to query texture size"
                  << std::endl;
        mDim = {0, 0};
    }
    if (fmod(mDim.x, mFrameCnt) != 0) {
        throw std::runtime_error(
            "RenderData::set(): " + std::to_string(mFrameCnt) +
            " does not evenly divide sprite sheet with length " +
            std::to_string(mDim.x));
    }
    mDim.x /= mFrameCnt;
    return setArea(Rect(0, 0, mDim.x, mDim.y)).setFit(mFit);
}
RenderData &RenderData::set(const std::string &file, unsigned int frameCnt) {
    return set(AssetManager::getTexture(file), frameCnt);
}
RenderData &RenderData::set(const TextData &tData, unsigned int frameCnt) {
    return set(tData.renderText(), frameCnt);
}
RenderData &RenderData::set(const AnimationData &animData) {
    return set(animData.file, animData.num_frames);
}

RenderData &RenderData::setDest(Rect r) {
    mRect = r;
    return setFit(mFit);
}
RenderData &RenderData::setArea(Rect r) {
    SDL_Rect result;
    SDL_IntersectRect(Rect(0, 0, mDim.x, mDim.y), r, &result);
    mArea = result;
    return *this;
}
RenderData &RenderData::setBoundary(Rect r) {
    mBounds = r;
    return *this;
}
RenderData &RenderData::addBoundary(Rect r) {
    SDL_Rect result;
    SDL_IntersectRect(mBounds, r, &result);
    mBounds = result;
    return *this;
}

RenderData &RenderData::setRotationRad(float rotation) {
    return setRotationDeg(rotation * 180 / M_PI);
}
RenderData &RenderData::setRotationDeg(float rotation) {
    mRotation = rotation;
    return *this;
}

RenderData &RenderData::setFit(FitMode fit) {
    mFit = fit;
    switch (mFit) {
        case FitMode::Dest:
            mDest = mRect;
            break;
        case FitMode::Fit:
            mDest = Rect::getMinRect(mDim.x, mDim.y, mRect.w(), mRect.h());
            mDest.setPos(mRect, mFitAlignX, mFitAlignY);
            break;
        case FitMode::Texture:
            mDest = Rect(0, 0, mDim.x, mDim.y);
            mDest.setPos(mRect, mFitAlignX, mFitAlignY);
            break;
    }
    return *this;
}
RenderData &RenderData::setFitAlign(Rect::Align a) { return setFitAlign(a, a); }
RenderData &RenderData::setFitAlign(Rect::Align aX, Rect::Align aY) {
    mFitAlignX = aX;
    mFitAlignY = aY;
    return setFit(mFit);
}

const Rect &RenderData::getRect() const { return mRect; }
const Rect &RenderData::getDest() const { return mDest; }

unsigned int RenderData::getNumFrames() const { return mFrameCnt; }
unsigned int RenderData::getFrame() const { return mFrame; }
void RenderData::setFrame(unsigned int frame) { mFrame = frame % mFrameCnt; }
void RenderData::nextFrame() { setFrame(mFrame + 1); }

void RenderData::draw(TextureBuilder &tex) const {
    int w, h;
    Renderer::getTargetSize(&w, &h);
    Rect renderBounds(0, 0, w, h);

    // Make sure we are actually drawing something
    if (mDest.empty()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Empty destination rect" << std::endl;
#endif
        return;
    }
    // Check the texture to draw
    if (!mTex) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Invalid Texture" << std::endl;
#endif
        return;
    }
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
    float leftFrac = fmaxf(boundary.x() - mDest.x(), 0) / mDest.w();
    float topFrac = fmaxf(boundary.y() - mDest.y(), 0) / mDest.h();
    float rightFrac = fmaxf(mDest.x2() - boundary.x2(), 0) / mDest.w();
    float botFrac = fmaxf(mDest.y2() - boundary.y2(), 0) / mDest.h();
    // Make sure the rect is actually in the boundary
    if (leftFrac + rightFrac >= 1 || topFrac + botFrac >= 1) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Rect " << destRect
                  << " was out side the boundary " << boundary << std::endl;
#endif
        return;
    }

    Rect destRect(mDest.x() + mDest.w() * leftFrac,
                  mDest.y() + mDest.h() * topFrac,
                  mDest.w() * (1. - leftFrac - rightFrac),
                  mDest.h() * (1. - topFrac - botFrac));

    Rect texRect(mArea.x() + mArea.w() * leftFrac + mDim.x * mFrame,
                 mArea.y() + mArea.h() * topFrac,
                 mArea.w() * (1.f - leftFrac - rightFrac),
                 mArea.h() * (1.f - topFrac - botFrac));
    // Make sure at least one pixel will be drawn
    if (texRect.empty()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Can't draw from " << texRect << " to " << destRect
                  << std::endl;
#endif
        return;
    }

    SDL_RenderCopyEx(Renderer::get(), mTex.get(), texRect, destRect, mRotation,
                     NULL, SDL_FLIP_NONE);
}
