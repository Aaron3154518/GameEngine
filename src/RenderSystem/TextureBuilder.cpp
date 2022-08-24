#include "TextureBuilder.h"

#define RENDER_DEBUG

// TextureBuilder
TextureBuilder::TextureBuilder(int w, int h, SDL_Color bkgrnd) {
    reset(w, h, bkgrnd);
}
TextureBuilder::TextureBuilder(const std::string &src)
    : TextureBuilder(AssetManager::getTexture(src), true) {}
TextureBuilder::TextureBuilder(SharedTexture src, bool copy) {
    if (copy) {
        SDL_Point dim = getTextureSize(src.get());
        reset(dim.x, dim.y);

        RenderData data;
        data.dest = Rect(0, 0, dim.x, dim.y);
        data.texture = src;
        draw(data);
    } else {
        mTex = src;
    }
}

// Get texture
SharedTexture TextureBuilder::getTexture() { return mTex; }

// Start new texture
void TextureBuilder::reset(int w, int h, SDL_Color bkgrnd) {
    mTex = makeSharedTexture(SDL_CreateTexture(Renderer::get(),
                                               SDL_PIXELFORMAT_RGBA8888,
                                               SDL_TEXTUREACCESS_TARGET, w, h));
    SDL_SetTextureBlendMode(mTex.get(), SDL_BLENDMODE_BLEND);
    RectData r;
    r.color = bkgrnd;
    r.blendMode = SDL_BLENDMODE_BLEND;
    draw(r.set());
}

// Draw textures/text
void TextureBuilder::draw(const RenderData &data) {
    Renderer::setRenderTarget(mTex.get());

    int w, h;
    Renderer::getTargetSize(&w, &h);
    Rect renderBounds(0, 0, w, h);

    // Make sure we are actually drawing something
    if (data.dest.empty()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Empty destination rect" << std::endl;
#endif
        return;
    }
    // Check the texture to draw
    if (!data.texture) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Invalid Texture" << std::endl;
#endif
        return;
    }
    // Get the boundary rect
    Rect boundary = data.boundary;
    SDL_Rect result;
    if (boundary.empty()) {
        boundary = renderBounds;
    } else if (SDL_IntersectRect(boundary, renderBounds, &result) ==
               SDL_FALSE) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Boundary rect " << boundary
                  << " was outside the screen: " << renderBounds << std::endl;
#endif
        return;
    }

    Rect destRect = data.dest;
    // Get fraction of item to draw
    float leftFrac = fmax(boundary.x() - destRect.x(), 0) / destRect.w();
    float topFrac = fmax(boundary.y() - destRect.y(), 0) / destRect.h();
    float rightFrac = fmax(destRect.x2() - boundary.x2(), 0) / destRect.w();
    float botFrac = fmax(destRect.y2() - boundary.y2(), 0) / destRect.h();
    // Make sure the rect is actually in the boundary
    if (leftFrac + rightFrac >= 1 || topFrac + botFrac >= 1) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Rect " << destRect
                  << " was out side the boundary " << boundary << std::endl;
#endif
        return;
    }

    destRect = Rect(destRect.x() + destRect.w() * leftFrac,
                    destRect.y() + destRect.h() * topFrac,
                    destRect.w() * (1. - leftFrac - rightFrac),
                    destRect.h() * (1. - topFrac - botFrac));

    SDL_Point srcDim = getTextureSize(data.texture.get());
    Rect areaRect =
        data.area.empty() ? Rect(0, 0, srcDim.x, srcDim.y) : data.area;
    Rect texRect = Rect(areaRect.x() + areaRect.w() * leftFrac,
                        areaRect.y() + areaRect.h() * topFrac,
                        areaRect.w() * (1. - leftFrac - rightFrac),
                        areaRect.h() * (1. - topFrac - botFrac));
    // Make sure at least one pixel will be drawn
    if (texRect.empty()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Can't draw from " << texRect << " to " << destRect
                  << std::endl;
#endif
        return;
    }

    SDL_RenderCopy(Renderer::get(), data.texture.get(), texRect, destRect);

    Renderer::resetRenderTarget();
}
// Draw shapes
void TextureBuilder::startDrawShape(const ShapeData &data) {
    Renderer::setRenderTarget(mTex.get());
    Renderer::setBlendMode(data.blendMode);
    Renderer::setDrawColor(data.color);
}
void TextureBuilder::draw(const ShapeData &data) {
    startDrawShape(data);
    data.draw(*this);
    endDrawShape();
}
void TextureBuilder::endDrawShape() {
    Renderer::resetDrawColor();
    Renderer::resetBlendMode();
    Renderer::resetRenderTarget();
}

// Brighten texture
void TextureBuilder::brighten(Uint8 strength) {
    RectData r;
    r.color = SDL_Color{strength, strength, strength, 255};
    r.blendMode = SDL_BLENDMODE_ADD;
    draw(r.set());
}

SDL_Point TextureBuilder::getTextureSize() {
    return getTextureSize(mTex.get());
}
SDL_Point TextureBuilder::getTextureSize(SDL_Texture *tex) {
    SDL_Point p;
    if (SDL_QueryTexture(tex, NULL, NULL, &p.x, &p.y) != 0) {
#ifdef RENDER_DEBUG
        std::cerr << "getTextureSize(): "
                  << "could not query source texture" << std::endl;
#endif
        p.x = p.y = 0;
    }
    return p;
}