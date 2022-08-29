#include "TextRender.h"

// TextData
// Functions to render text to a texture
TextData& TextData::setText(const std::string& text) {
    return setText(text, 0, {});
}
TextData& TextData::setText(const std::string& text, int w,
                            const std::initializer_list<RenderData>& imgs) {
    mText = text;
    mW = w;
    mImgs = imgs;
}
TextData& TextData::setTextImgs(const std::initializer_list<RenderData>& imgs) {
    if (imgs.size() != mImgs.size()) {
        std::cerr << "TextData::setTextImgs(): Received " << imgs.size()
                  << " images but expected " << mImgs.size()
                  << "Call setText() to change the number of images"
                  << std::endl;
        return *this;
    }
    mImgs = imgs;
    return *this;
}

SharedTexture TextData::renderText() const {
    if (mW <= 0) {
        return renderTextLine();
    } else {
        return renderTextWrapped();
    }
}
SharedTexture TextData::renderTextLine() const {
    SharedTexture tex = makeSharedTexture();
    if (mText.empty()) {
        return tex;
    }
    if (!font) {
        std::cerr << "renderText(): Invalid font" << std::endl;
        return tex;
    }
    Surface surface = makeSurface();
    if (bkgrnd == TRANSPARENT) {
        surface.reset(TTF_RenderText_Blended(font.get(), mText.c_str(), color));
    } else {
        surface.reset(
            TTF_RenderText_Shaded(font.get(), mText.c_str(), color, bkgrnd));
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
    std::string textCopy = mText;
    std::unique_ptr<std::list<LinePtr>> lines =
        splitText(textCopy, font, mW, mImgs);
    if (lines->empty()) {
        return makeSharedTexture();
    }

    int maxLineW = 0;
    if (autoFit) {
        for (auto& line : *lines) {
            if (line->mW > maxLineW) {
                maxLineW = line->mW;
            }
        }
    } else {
        maxLineW = mW;
    }

    int lineH = TTF_FontHeight(font.get());
    int h = lineH * lines->size();
    TextureBuilder tex(maxLineW, h, bkgrnd);
    Rect lineR(0, 0, maxLineW, lineH);
    for (auto& line : *lines) {
        if (!line->empty()) {
            line->draw(tex, lineR, *this, textCopy);
        }
        lineR.move(0, lineH);
    }
    return tex.getTexture();
}

// Element
Element::Element(int w) : mW(w) {}

void Element::draw(TextureBuilder& tex, Rect rect, const TextData& td,
                   std::string& text) {}

// Text
Text::Text(int startPos, int len, int w)
    : Element(w), mStartPos(startPos), mLen(len) {}

void Text::draw(TextureBuilder& tex, Rect rect, const TextData& td,
                std::string& text) {
    Surface textSurf = makeSurface();
    size_t endPos = mStartPos + mLen;
    if (endPos != text.size()) {
        char tmp = text.at(endPos);
        text.replace(endPos, 1, 1, '\0');
        textSurf = makeSurface(TTF_RenderText_Blended(
            td.font.get(), text.c_str() + mStartPos, td.color));
        text.replace(endPos, 1, 1, tmp);
    } else {
        textSurf = makeSurface(TTF_RenderText_Blended(
            td.font.get(), text.c_str() + mStartPos, td.color));
    }

    SharedTexture textTex = makeSharedTexture(
        SDL_CreateTextureFromSurface(Renderer::get(), textSurf.get()));
    tex.draw(RenderData()
                 .set(textTex)
                 .setFit(RenderData::FitMode::Texture)
                 .setFitAlign(td.align, Rect::Align::CENTER)
                 .setDest(rect));
}

// Image
Image::Image(const RenderData& img, int lineH) : Element(lineH), mImg(img) {}

void Image::draw(TextureBuilder& tex, Rect rect, const TextData& td,
                 std::string& text) {
    tex.draw(mImg.setDest(rect));
}

// Line
bool Line::empty() const { return mElements.empty(); }

int Line::getSpace(int maxW) const { return maxW - mW; }

void Line::addElement(ElementPtr e) {
    mW += e->mW;
    mElements.push_back(std::move(e));
}

void Line::draw(TextureBuilder& tex, Rect rect, const TextData& td,
                std::string& text) {
    Rect lineRect(0, 0, mW, rect.h());
    lineRect.setPos(rect, td.align, Rect::Align::CENTER);
    for (auto& e : mElements) {
        lineRect.setDim(e->mW, rect.h(), Rect::Align::TOP_LEFT,
                        Rect::Align::CENTER);
        e->draw(tex, lineRect, td, text);
        lineRect.move(e->mW, 0);
    }
}

// splitText
std::unique_ptr<std::list<LinePtr>> splitText(
    std::string& text, SharedFont font, int maxW,
    const std::vector<RenderData>& imgs) {
    std::unique_ptr<std::list<LinePtr>> lines =
        std::make_unique<std::list<LinePtr>>();
    lines->push_back(std::make_unique<Line>());
    if (!font) {
        return lines;
    }

    int lineH = TTF_FontHeight(font.get());
    int spaceW;
    TTF_SizeText(font.get(), " ", &spaceW, nullptr);

    std::function<void(size_t, size_t)> addText = [&lines, &font, &text,
                                                   &addText, maxW,
                                                   spaceW](size_t pos1,
                                                           size_t pos2) {
        if (pos1 >= pos2) {
            return;
        }

        const char* str = text.c_str() + pos1;
        size_t len = pos2 - pos1;

        int count = 0, width = 0;
        if (pos2 != text.size()) {
            char tmp = text.at(pos2);
            text.replace(pos2, 1, 1, '\0');
            TTF_MeasureUTF8(font.get(), str, lines->back()->getSpace(maxW),
                            &width, &count);
            text.replace(pos2, 1, 1, tmp);
        } else {
            TTF_MeasureUTF8(font.get(), str, lines->back()->getSpace(maxW),
                            &width, &count);
        }

        if (count == len) {  // Fit entire text onto line
            lines->back()->addElement(std::make_unique<Text>(pos1, len, width));
        } else {  // Break up text
            int idx;
            // Find last space, if any
            for (idx = count - 1; idx >= 0; idx--) {
                if (*(str + idx) == ' ') {
                    break;
                }
            }
            if (idx != -1) {  // Break into words
                int textW;

                text.replace(pos1 + idx, 1, 1, '\0');
                TTF_SizeText(font.get(), str, &textW, nullptr);
                text.replace(pos1 + idx, 1, 1, ' ');

                auto ptr = std::make_unique<Text>(pos1, idx, textW);
                lines->back()->addElement(std::move(ptr));
                lines->push_back(std::make_unique<Line>());
                addText(pos1 + idx + 1, pos2);
            } else {  // Won't fit on this line
                // Get the length until the next break
                int wordW = 0;
                auto spaceIt =
                    std::find(text.begin() + pos1, text.begin() + pos2, ' ');
                if (spaceIt == text.end()) {
                    TTF_SizeUTF8(font.get(), str, &wordW, nullptr);
                } else {
                    char tmp = *spaceIt;
                    *spaceIt = '\0';
                    TTF_SizeUTF8(font.get(), str, &wordW, nullptr);
                    *spaceIt = tmp;
                }
                if (wordW <= maxW) {  // It will fit on the next line
                    lines->push_back(std::make_unique<Line>());
                    addText(pos1, pos2);
                } else {  // It is bigger than one line, split accross multiple
                          // lines
                    lines->back()->addElement(
                        std::make_unique<Text>(pos1, count, width));
                    lines->push_back(std::make_unique<Line>());
                    addText(pos1 + count, pos2);
                }
            }
        }
    };

    std::string delims = "\n{";
    size_t pos = 0, pos2, idx = text.find_first_of(delims, pos);
    int backetCnt = 0, imgIdx = 0;
    while (idx != std::string::npos) {
        addText(pos, idx);
        switch (text.at(idx)) {
            case '\n':
                lines->push_back(std::make_unique<Line>());
                break;
            case '{':
                pos = idx + 1;
                idx = text.find('}', pos);
                if (idx == std::string::npos) {
                    std::cerr << "splitText(): Unterminated '{'" << std::endl;
                    return lines;
                }
                if (idx == pos) {
                    break;
                }
                switch (text.at(pos)) {
                    case 'b':
                        break;
                    case 'i':
                        if (imgs.size() <= imgIdx) {
                            std::cerr
                                << "splitText(): Not enough images provided"
                                << std::endl;
                            break;
                        }
                        if (lineH > lines->back()->getSpace(maxW)) {
                            lines->push_back(std::make_unique<Line>());
                        }
                        lines->back()->addElement(
                            std::make_unique<Image>(imgs.at(imgIdx++), lineH));
                        break;
                };
                break;
        }
        pos = idx + 1;
        idx = text.find_first_of(delims, pos);
    }
    addText(pos, text.size());

    return lines;
}
