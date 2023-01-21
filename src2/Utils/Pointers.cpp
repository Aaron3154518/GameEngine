#include "Pointers.h"

// Because TTF is stupid and needs to be initialized to close fonts
// Fonts will not be closed after quitting TTF
// This only happens when the program is about to end
void safeCloseFont(TTF_Font *font) {
    if (TTF_WasInit()) {
        TTF_CloseFont(font);
    }
}

// Memory management functions
Surface makeSurface(SDL_Surface *surf) {
    return Surface(surf, SDL_FreeSurface);
}
SharedSurface makeSharedSurface(SDL_Surface *surf) {
    return SharedSurface(surf, SDL_FreeSurface);
}

Texture makeTexture(SDL_Texture *tex) {
    return Texture(tex, SDL_DestroyTexture);
}
SharedTexture makeSharedTexture(SDL_Texture *tex) {
    return SharedTexture(tex, SDL_DestroyTexture);
}

Font makeFont(TTF_Font *font) { return Font(font, safeCloseFont); }
SharedFont makeSharedFont(TTF_Font *font) {
    return SharedFont(font, safeCloseFont);
}
