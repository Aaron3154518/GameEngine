#include "TextComponent.h"

// Text
Text::Text(int startPos, int len, int w)
    : mStartPos(startPos), mLen(len), mW(w) {
    addComponent<PositionComponent>(Rect());
    addComponent<ElevationComponent>(5);
    addComponent<SpriteComponent>("");
    addComponent<RenderService>();
}

int Text::w() const { return mW; }

void Text::draw(Rect rect, const SharedFont& font, std::string& text) {
    Surface textSurf = makeSurface();
    size_t endPos = mStartPos + mLen;
    if (endPos != text.size()) {
        char tmp = text.at(endPos);
        text.replace(endPos, 1, 1, '\0');
        textSurf = makeSurface(TTF_RenderText_Blended(
            font.get(), text.c_str() + mStartPos, Colors::Black));
        text.replace(endPos, 1, 1, tmp);
    } else {
        textSurf = makeSurface(TTF_RenderText_Blended(
            font.get(), text.c_str() + mStartPos, Colors::Black));
    }

    SharedTexture textTex = makeSharedTexture(
        SDL_CreateTextureFromSurface(Renderer::get(), textSurf.get()));

    // rd.setFit(RenderData::FitMode::Texture);
    // rd.setFitAlign(tData.mAlign, Rect::Align::CENTER);

    getComponent<PositionComponent>().set(rect);
    addComponent<SpriteComponent>(textTex);
}

// Image
Image::Image(int lineH) : mW(lineH) {
    addComponent<PositionComponent>(Rect());
    addComponent<ElevationComponent>(5);
    addComponent<SpriteComponent>("");
    addComponent<RenderService>();
}

int Image::w() const { return mW; }

void Image::draw(Rect rect, const std::string& img) {
    getComponent<PositionComponent>().set(rect);
    addComponent<SpriteComponent>(img);
}

// Line
bool Line::empty() const { return mTypes.empty(); }

int Line::w() const { return mW; }

int Line::getSpace(int maxW) const { return maxW - mW; }

void Line::addElement(TextPtr text) {
    mW += text->w();
    mText.push_back(std::move(text));
    mTypes.push_back(Type::TEXT);
}
void Line::addElement(ImagePtr img) {
    mW += img->w();
    mImgs.push_back(std::move(img));
    mTypes.push_back(Type::IMAGE);
}

void Line::drawText(Rect rect, const SharedFont& font, std::string& text) {
    Rect lineRect(0, 0, mW, rect.h());
    // LEFT: td mAlign
    lineRect.setPos(rect, Rect::Align::TOP_LEFT, Rect::Align::CENTER);
    auto textIt = mText.begin();
    auto imgIt = mImgs.begin();
    for (auto type : mTypes) {
        switch (type) {
            case Type::TEXT: {
                TextPtr& t = *(textIt++);
                lineRect.setDim(t->w(), rect.h(), Rect::Align::TOP_LEFT,
                                Rect::Align::CENTER);
                t->draw(lineRect, font, text);
                lineRect.move(t->w(), 0);
            } break;
            case Type::IMAGE:
                lineRect.move((*(imgIt++))->w(), 0);
                break;
        };
    }
}
size_t Line::drawImages(Rect rect, const std::vector<std::string>& imgs,
                        size_t startPos) {
    if (mImgs.size() > imgs.size() - startPos) {
        std::cerr << "Line::drawImages(): Expected " << mImgs.size()
                  << " images but received " << (imgs.size() - startPos)
                  << std::endl;
        return startPos;
    }
    Rect lineRect(0, 0, mW, rect.h());
    // LEFT should be text data align
    lineRect.setPos(rect, Rect::Align::TOP_LEFT, Rect::Align::CENTER);
    auto textIt = mText.begin();
    auto imgIt = mImgs.begin();
    for (auto type : mTypes) {
        switch (type) {
            case Type::TEXT:
                lineRect.move((*(textIt++))->w(), 0);
                break;
            case Type::IMAGE: {
                ImagePtr& i = *(imgIt++);
                lineRect.setDim(i->w(), rect.h(), Rect::Align::TOP_LEFT,
                                Rect::Align::CENTER);
                // auto rectShape = RectShape(td.data().mBkgrnd).set(lineRect);
                // tex.draw(rectShape);
                i->draw(lineRect, imgs.at(startPos++));
                lineRect.move(i->w(), 0);
            } break;
        };
    }
    return startPos;
}

// splitText
std::list<Line> splitText(std::string& text, SharedFont font, int maxW) {
    std::list<Line> lines;
    lines.push_back(Line());
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
            TTF_MeasureUTF8(font.get(), str, lines.back().getSpace(maxW),
                            &width, &count);
            text.replace(pos2, 1, 1, tmp);
        } else {
            TTF_MeasureUTF8(font.get(), str, lines.back().getSpace(maxW),
                            &width, &count);
        }

        if (count == len) {  // Fit entire text onto line
            lines.back().addElement(std::make_unique<Text>(pos1, len, width));
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

                lines.back().addElement(
                    std::make_unique<Text>(pos1, idx, textW));
                lines.push_back(Line());
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
                    lines.push_back(Line());
                    addText(pos1, pos2);
                } else {  // It is bigger than one line, split accross
                          // multiple lines
                    lines.back().addElement(
                        std::make_unique<Text>(pos1, count, width));
                    lines.push_back(Line());
                    addText(pos1 + count, pos2);
                }
            }
        }
    };

    std::string delims = "\n{";
    size_t pos = 0, idx = text.find_first_of(delims, pos);
    while (idx != std::string::npos) {
        addText(pos, idx);
        switch (text.at(idx)) {
            case '\n':
                lines.push_back(Line());
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
                        if (lineH > lines.back().getSpace(maxW)) {
                            lines.push_back(Line());
                        }
                        lines.back().addElement(std::make_unique<Image>(lineH));
                        break;
                };
                break;
        }
        pos = idx + 1;
        idx = text.find_first_of(delims, pos);
    }
    addText(pos, text.size());

    return std::move(lines);
}

// TextData
TextData::TextData(const Rect& rect, const std::string& text,
                   const std::vector<std::string>& imgs)
    : mImgs(imgs), mText(text), mRect(rect) {
    SharedFont font = AssetManager::getFont({-1, 10, "|"});
    mLines = splitText(mText, font, mRect.W());
    Rect lineR = mRect;
    lineR.setHeight(lineR.h() / mLines.size());
    size_t p = 0;
    for (auto& line : mLines) {
        line.drawText(lineR, font, mText);
        p = line.drawImages(lineR, mImgs, p);
        lineR.move(0, lineR.h());
    }
}
