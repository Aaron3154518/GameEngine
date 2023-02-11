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
        SDL_Point dim = getTextureSize(src.get());
        reset(dim.x, dim.y);

        RenderData rd(src);
        rd.mRect = Rect(0, 0, dim.x, dim.y);
        draw(rd);
    } else {
        mTex = src;
    }
}

// Get texture
SharedTexture TextureBuilder::getTexture() { return mTex; }

// Start new texture
void TextureBuilder::reset(int w, int h, SDL_Color bkgrnd) {
    SDL_Texture *tex =
        SDL_CreateTexture(Renderer::get(), SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, w, h);
    mTex = makeSharedTexture(tex);
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
}

// Brighten texture
void TextureBuilder::brighten(Uint8 strength) {
    Shapes::Rectangle r;
    r.setColor(SDL_Color{strength, strength, strength, 255});
    r.setBlendMode(SDL_BLENDMODE_ADD);
    draw(r);
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