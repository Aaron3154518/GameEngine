#include "TextureBuilder.h"

#include <Framework/RenderSystem/RenderData.h>
#include <Framework/RenderSystem/Shapes.h>

// Drawable
void Drawable::draw(TextureBuilder &tex) {}

// TextureBuilder
TextureBuilder::TextureBuilder(int w, int h, SDL_Color bkgrnd) {
    reset(w, h, bkgrnd);
}
TextureBuilder::TextureBuilder(const std::string &src)
    : TextureBuilder(AssetManager::getTexture(src), true) {}
TextureBuilder::TextureBuilder(SharedTexture src, bool copy) {
    if (copy) {
        Dimensions dim = getTextureSize(src.get());
        reset(dim.w, dim.h);

        RenderData rd(src);
        rd.mRect = Rect(0, 0, dim.w, dim.h);
        draw(rd);
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
    Shapes::Rectangle r;
    r.setColor(bkgrnd);
    r.setBlendMode(SDL_BLENDMODE_NONE);
    draw(r);
}

// Draw textures/text
void TextureBuilder::draw(Drawable &drawable) {
    Renderer::setRenderTarget(mTex.get());
    drawable.draw(*this);
    Renderer::setRenderTarget(nullptr);
}

// Brighten texture
void TextureBuilder::brighten(Uint8 strength) {
    Shapes::Rectangle r;
    r.setColor(SDL_Color{strength, strength, strength, 255});
    r.setBlendMode(SDL_BLENDMODE_ADD);
    draw(r);
}

Dimensions TextureBuilder::getTextureSize() {
    return getTextureSize(mTex.get());
}
Dimensions TextureBuilder::getTextureSize(SDL_Texture *tex) {
    Dimensions d;
    if (SDL_QueryTexture(tex, NULL, NULL, &d.w, &d.h) != 0) {
#ifdef RENDER_DEBUG
        std::cerr << "getTextureSize(): "
                  << "could not query source texture" << std::endl;
#endif
        d.w = d.h = 0;
    }
    return d;
}