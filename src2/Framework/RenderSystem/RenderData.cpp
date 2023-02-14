#include "RenderData.h"

// RenderData
RenderData::RenderData() {}
RenderData::RenderData(SharedTexture tex) : mTex(tex) {}

// RenderData
void RenderData::draw(TextureBuilder& tex) {
    if (!mTex) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Invalid Texture" << std::endl;
#endif
        return;
    }

    Dimensions texDim = AssetManager::getTextureSize(mTex.get());
    Rect area(mArea.x() * texDim.w, mArea.y() * texDim.h, mArea.w() * texDim.w,
              mArea.h() * texDim.h);
    Rect boundary = getBounds();

    // Make sure we are actually drawing something
    if (mRect.empty() || area.empty() || boundary.empty()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Empty destination rect" << std::endl;
#endif
        return;
    }

    // Get fraction of item to draw
    float leftFrac = fmaxf(boundary.x() - mRect.x(), 0) / mRect.w();
    float topFrac = fmaxf(boundary.y() - mRect.y(), 0) / mRect.h();
    float rightFrac = fmaxf(mRect.x2() - boundary.x2(), 0) / mRect.w();
    float botFrac = fmaxf(mRect.y2() - boundary.y2(), 0) / mRect.h();

    Rect dest(mRect.x() + mRect.w() * leftFrac, mRect.y() + mRect.h() * topFrac,
              mRect.w() * (1. - leftFrac - rightFrac),
              mRect.h() * (1. - topFrac - botFrac));

    // Make sure the mRect is actually in the boundary
    if (leftFrac + rightFrac >= 1 || topFrac + botFrac >= 1) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Rect " << dest << " was out side the boundary "
                  << boundary << std::endl;
#endif
        return;
    }

    area = Rect(area.x() + area.w() * leftFrac, area.y() + area.h() * topFrac,
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

    SDL_RenderCopyEx(Renderer::get(), mTex.get(), area, dest,
                     0,  // mRotation,
                     NULL, SDL_FLIP_NONE);
}

Rect RenderData::getMinRect(const Rect& r) {
    return Rect::getMinRect(mTex.get(), r.w(), r.h());
}
