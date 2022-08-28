#include "RenderTypes.h"

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
Texture TextData::renderText() const {
    if (w <= 0) {
        return renderTextLine();
    } else {
        return renderTextWrapped();
    }
}
Texture TextData::renderTextLine() const {
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
Texture TextData::renderTextWrapped() const {
    Texture tex = makeTexture();
    if (!font) {
        std::cerr << "renderTextWrapped(): Could not load font" << std::endl;
        return tex;
    }
    std::vector<std::string> lines = splitText(text, font, w);
    if (lines.empty()) {
        return tex;
    }

    int maxLineW = 0;
    if (autoFit) {
        int lineW;
        for (auto line : lines) {
            TTF_SizeText(font.get(), line.c_str(), &lineW, NULL);
            if (lineW > maxLineW) {
                maxLineW = lineW;
            }
        }
    } else {
        maxLineW = w;
    }

    // TODO: Switch to all SDL_Texture* (faster)?
    int lineH = TTF_FontHeight(font.get());
    int h = lineH * lines.size();
    Surface surf = makeSurface(
        SDL_CreateRGBSurface(0, maxLineW, h, 32, rmask, gmask, bmask, amask));
    if (bkgrnd != TRANSPARENT) {
        SDL_FillRect(surf.get(), NULL, toUint(bkgrnd));
    }
    Rect lineR(0, 0, maxLineW, lineH);
    for (auto line : lines) {
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
RenderData &RenderData::set(SharedTexture tex, unsigned int frameCnt) {
    mTex = tex;
    mFrameCnt = frameCnt;
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
