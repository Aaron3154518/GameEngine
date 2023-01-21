#include "Renderer.h"

RendererPtr Renderer::mRenderer = RendererPtr(NULL, SDL_DestroyRenderer);

// Renderer functions
void Renderer::init(SDL_Window *window) {
    if (!window) {
        std::cerr << "Invalid SDL_Window for initialization" << std::endl;
        return;
    }
    mRenderer.reset(SDL_CreateRenderer(window, -1, 0));
}
SDL_Renderer *Renderer::get(bool notNull) {
    if (!mRenderer && notNull) {
        throw NullRendererException();
    }
    return mRenderer.get();
}
SDL_Point Renderer::getTargetSize() {
    SDL_Point size;
    getTargetSize(&size.x, &size.y);
    return size;
}
void Renderer::getTargetSize(int *w, int *h) {
    SDL_GetRendererOutputSize(mRenderer.get(), w, h);
}

void Renderer::setRenderTarget(SDL_Texture *tex) {
    SDL_SetRenderTarget(mRenderer.get(), tex);
}
void Renderer::setDrawColor(const SDL_Color &c) {
    SDL_SetRenderDrawColor(mRenderer.get(), c.r, c.g, c.b, c.a);
}
void Renderer::setBlendMode(SDL_BlendMode mode) {
    SDL_SetRenderDrawBlendMode(mRenderer.get(), mode);
}
