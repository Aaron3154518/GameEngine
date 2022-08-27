#include "TextureBuilder.h"

#include "RenderTypes.h"
#include "Shapes.h"

#define RENDER_DEBUG

// Drawable
Drawable::Drawable(SDL_Color color, SDL_BlendMode mode)
    : mColor(color), mBlendMode(mode) {}
Drawable::Drawable(SDL_BlendMode mode, SDL_Color color)
    : Drawable(color, mode) {}
void Drawable::draw(TextureBuilder &tex) const {}

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

        draw(RenderData().set(src).setDest(Rect(0, 0, dim.x, dim.y)));
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
    draw(RectShape(bkgrnd, SDL_BLENDMODE_BLEND).set());
}

// Draw textures/text
void TextureBuilder::draw(const Drawable &drawable) {
    Renderer::setRenderTarget(mTex.get());
    Renderer::setDrawColor(drawable.mColor);
    Renderer::setBlendMode(drawable.mBlendMode);

    drawable.draw(*this);

    Renderer::resetRenderTarget();
    Renderer::resetDrawColor();
    Renderer::resetBlendMode();
}

// Brighten texture
void TextureBuilder::brighten(Uint8 strength) {
    draw(RectShape(SDL_Color{strength, strength, strength, 255},
                   SDL_BLENDMODE_ADD)
             .set());
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