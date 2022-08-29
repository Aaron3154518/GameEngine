#include "TextRender.h"

#include <RenderSystem/RenderTypes.h>

// Element
Element::Element(int w) : mW(w) {}

void Element::draw(TextureBuilder& tex, Rect rect, Rect::Align align,
                   TTF_Font* font, std::string& text, SDL_Color color) const {}

// Text
Text::Text(int startPos, int len, int w)
    : Element(w), mStartPos(startPos), mLen(len) {}

void Text::draw(TextureBuilder& tex, Rect rect, Rect::Align align,
                TTF_Font* font, std::string& text, SDL_Color color) const {
    Surface textSurf = makeSurface();
    size_t endPos = mStartPos + mLen;
    if (endPos != text.size()) {
        char tmp = text.at(endPos);
        text.replace(endPos, 1, 1, '\0');
        textSurf = makeSurface(
            TTF_RenderText_Blended(font, text.c_str() + mStartPos, color));
        text.replace(endPos, 1, 1, tmp);
    } else {
        textSurf = makeSurface(
            TTF_RenderText_Blended(font, text.c_str() + mStartPos, color));
    }

    SharedTexture textTex = makeSharedTexture(
        SDL_CreateTextureFromSurface(Renderer::get(), textSurf.get()));
    Rect lineRect = Rect(0, 0, textSurf->w, textSurf->h);
    lineRect.setPos(rect, align, Rect::Align::CENTER);
    tex.draw(RenderData().set(textTex).setDest(lineRect));
}

// Image
Image::Image(const std::string& img, int lineH) : Element(lineH), mImg(img) {}

void Image::draw(TextureBuilder& tex, Rect rect, Rect::Align align,
                 TTF_Font* font, std::string& text, SDL_Color color) const {
    SharedTexture imgTex = AssetManager::getTexture(mImg);
    SDL_FPoint c = rect.getPos(Rect::Align::CENTER);
    rect = Rect::getMinRect(imgTex.get(), rect.w(), rect.h());
    rect.setPos(c.x, c.y, Rect::Align::CENTER);
    tex.draw(RenderData().set(imgTex).setDest(rect));
}

// Line
bool Line::empty() const { return mElements.empty(); }

int Line::getSpace(int maxW) const { return maxW - mW; }

void Line::addElement(ElementPtr e) {
    mW += e->mW;
    mElements.push_back(std::move(e));
}

void Line::draw(TextureBuilder& tex, Rect rect, Rect::Align align,
                TTF_Font* font, std::string& text, SDL_Color color) const {
    Rect lineRect(0, 0, mW, rect.h());
    lineRect.setPos(rect, align, Rect::Align::CENTER);

    Rect elRect;
    for (auto& e : mElements) {
        lineRect.setDim(e->mW, rect.h(), Rect::Align::TOP_LEFT,
                        Rect::Align::CENTER);
        e->draw(tex, lineRect, align, font, text, color);
        lineRect.move(e->mW, 0);
    }
}

// splitText
std::unique_ptr<std::list<LinePtr>> splitText2(std::string& text,
                                               SharedFont font, int maxW) {
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
    int backetCnt = 0;
    do {
        addText(pos, idx);
        switch (text.at(idx)) {
            case '\n':
                lines->push_back(std::make_unique<Line>());
                break;
            case '{':
                pos = idx + 1;
                idx = text.find('}', pos);
                if (idx == std::string::npos) {
                    return lines;
                }
                if (idx == pos) {
                    break;
                }
                switch (text.at(pos)) {
                    case 'b':
                        break;
                    case 'i':
                        if (idx == pos + 1) {
                            break;
                        }
                        if (lineH > lines->back()->getSpace(maxW)) {
                            lines->push_back(std::make_unique<Line>());
                        }
                        lines->back()->addElement(std::make_unique<Image>(
                            text.substr(pos + 1, idx - (pos + 1)), lineH));
                        break;
                };
                break;
        }
        pos = idx + 1;
        idx = text.find_first_of(delims, pos);
    } while (idx != std::string::npos);
    addText(pos, text.size());

    return lines;
}
