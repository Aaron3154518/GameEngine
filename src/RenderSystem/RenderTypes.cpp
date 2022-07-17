#include "RenderTypes.h"

// Because TTF is stupid and needs to be initialized to close fonts
// Fonts will not be closed after quitting TTF however this only happens when
// the program is about to end
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

// Helper function to split text for wrapping
std::vector<std::string> splitText(const std::string &text, SharedFont font,
                                   int maxW) {
    std::vector<std::string> lines;
    if (!font) {
        return lines;
    }
    int maxChW = 0;
    TTF_SizeText(font.get(), "_", &maxChW, NULL);
    if (maxW < maxChW) {
        std::cerr << "splitText(): Specified width is too small. Is: " << maxW
                  << " Should be at least: " << maxChW << std::endl;
        return lines;
    }

    std::stringstream line_ss, word_ss;
    int spaceW, dashW;
    TTF_SizeText(font.get(), " ", &spaceW, NULL);
    TTF_SizeText(font.get(), "-", &dashW, NULL);
    int lineW = 0;
    bool addSpace = false;
    // Go through each character in the input
    for (char ch : text + '\n') {
        // Finished this word
        if (ch == ' ' || ch == '\n' || ch == '\b') {
            std::string word = word_ss.str();
            word_ss.str("");
            int wordW = 0;
            TTF_SizeText(font.get(), word.c_str(), &wordW, NULL);
            // This word does not overflow the line, add it
            if (lineW + wordW <= maxW) {
                // Add space if necessary
                if (addSpace) {
                    line_ss << ' ';
                }
                line_ss << word;
                lineW += wordW;
                if (ch == ' ') {
                    addSpace = true;
                    lineW += spaceW;
                } else {
                    addSpace = false;
                }
                // This word overflows the line, check solutions
            } else {
                int i = 0;
                // If the word will not fit on its own line, break it up
                while (wordW > maxW && i < word.size() - 1) {
                    int lb = i;
                    const char *begin = word.data() + lb;
                    char save = word[lb + 1];
                    word[lb + 1] = '\0';
                    int subW = 0;
                    TTF_SizeText(font.get(), begin, &subW, NULL);
                    while (lineW + subW + dashW <= maxW &&
                           i < word.size() - 2) {
                        ++i;
                        word[i] = save;
                        save = word[i + 1];
                        word[i + 1] = '\0';
                        TTF_SizeText(font.get(), begin, &subW, NULL);
                    }
                    // Get remaining word width
                    word[i + 1] = save;
                    TTF_SizeText(font.get(), word.data() + i, &wordW, NULL);
                    // Make sure we actually fit some characters
                    // from the word onto the line (not just the '-')
                    if (i > lb) {
                        // If adding to an existing line, check to add space
                        if (lineW > 0 && addSpace) {
                            line_ss << " ";
                        }
                        line_ss << word.substr(lb, i - lb) << "-";
                    }
                    lines.push_back(line_ss.str());
                    line_ss.str("");
                    lineW = 0;
                }
                // Word will fit on its own line
                if (i == 0) {
                    lines.push_back(line_ss.str());
                    line_ss.str("");
                }
                // Add whatever's left
                line_ss << word.substr(i);
                lineW = wordW;
                if (ch == ' ') {
                    addSpace = true;
                    lineW += spaceW;
                } else {
                    addSpace = false;
                }
            }
            if (ch == '\n') {
                lines.push_back(line_ss.str());
                line_ss.str("");
                lineW = 0;
                addSpace = false;
            }
            // Continue building word
        } else {
            word_ss << ch;
        }
    }
    return lines;
}

// TextData
// Functions to render text to a texture
Texture TextData::renderText() {
    if (w <= 0) {
        return renderTextLine();
    } else {
        return renderTextWrapped();
    }
}
Texture TextData::renderTextLine() {
    Texture tex = makeTexture();
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
    tex = makeTexture(
        SDL_CreateTextureFromSurface(Renderer::get(), surface.get()));
    return tex;
}
Texture TextData::renderTextWrapped() {
    Texture tex = makeTexture();
    if (!font) {
        std::cerr << "renderTextWrapped(): Could not load font" << std::endl;
        return tex;
    }
    std::vector<std::string> lines = splitText(text, font, w);
    if (lines.empty()) {
        return tex;
    }

    // TODO: Switch to all SDL_Texture* (faster)?
    int lineH = TTF_FontHeight(font.get());
    int h = lineH * lines.size();
    Surface surf = makeSurface(
        SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask));
    if (bkgrnd != TRANSPARENT) {
        SDL_FillRect(surf.get(), NULL, toUint(bkgrnd));
    }
    Rect lineR(0, 0, w, lineH);
    for (std::string line : lines) {
        if (line != "") {
            Surface lineSurf = makeSurface(
                TTF_RenderText_Blended(font.get(), line.c_str(), color));
            if (lineSurf) {
                Rect lineRect = Rect(0, 0, lineSurf->w, lineSurf->h);
                lineRect.setPos(lineR, align, Rect::Align::CENTER);
                SDL_Rect lineRectSDL = lineRect.toSDLRect();
                SDL_BlitSurface(lineSurf.get(), NULL, surf.get(), &lineRectSDL);
            } else {
#ifdef RENDER_DEBUG
                std::cerr << "line '" << line << "' produced a null surface"
                          << std::endl;
#endif
            }
        }
        lineR.move(0, lineH);
    }
    tex =
        makeTexture(SDL_CreateTextureFromSurface(Renderer::get(), surf.get()));
    return tex;
}

// RenderData
void RenderData::fitToTexture(Rect::Align align) {
    fitToTexture(dest.w(), dest.h(), align);
}
void RenderData::fitToTexture(int maxW, int maxH, Rect::Align align) {
    Rect tmp = dest;
    dest = Rect::getMinRect(texture.get(), maxW, maxH);
    dest.setPos(tmp, align);
}

// TextRenderData
void TextRenderData::renderText() { texture = tData.renderText(); }