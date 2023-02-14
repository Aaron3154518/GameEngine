#include "TextComponent.h"

// Text
Text::Text(int startPos, int len, int w)
    : mStartPos(startPos), mLen(len), mW(w) {}

int Text::w() const { return mW; }

void Text::draw(TextureBuilder& tex, Rect rect, const SharedFont& font,
                const std::string& text) {
    Surface textSurf = makeSurface();
    std::string str = text.substr(mStartPos, mLen);
    textSurf = makeSurface(
        TTF_RenderText_Blended(font.get(), str.c_str(), Colors::Black));

    RenderData rd(makeSharedTexture(
        SDL_CreateTextureFromSurface(Renderer::get(), textSurf.get())));
    rd.mRect = rd.getMinRect(rect);
    // rd.setFitAlign(tData.mAlign, Rect::Align::CENTER);
    rd.mRect.setPos(rect, Rect::Align::CENTER);
    tex.draw(rd);
}

// Image
Image::Image(int lineH) : mW(lineH) {
    addComponent<PositionComponent>(Rect());
    addComponent<ElevationComponent>(6);
    addComponent<SpriteComponent>(makeSharedTexture());
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

size_t Line::draw(TextureBuilder& tex, Rect rect, SDL_FPoint off,
                  DimensionsF scale, const SharedFont& font,
                  const std::string& text, const std::vector<std::string>& imgs,
                  size_t startPos, Rect::Align alignX, Rect::Align alignY) {
    if (mImgs.size() > imgs.size() - startPos) {
        std::cerr << "Line::drawImages(): Expected " << mImgs.size()
                  << " images but received " << (imgs.size() - startPos)
                  << std::endl;
        return startPos;
    }
    rect.setWidth(mW, alignX);
    float x = rect.x();
    auto textIt = mText.begin();
    auto imgIt = mImgs.begin();
    for (auto type : mTypes) {
        switch (type) {
            case Type::TEXT: {
                TextPtr& t = *(textIt++);
                t->draw(tex, Rect(x, rect.y(), t->w(), rect.h()), font, text);
                x += t->w();
            } break;
            case Type::IMAGE: {
                ImagePtr& i = *(imgIt++);
                i->draw(Rect(x * scale.w + off.x, rect.y() * scale.h + off.y,
                             i->w() * scale.h, rect.h() * scale.h),
                        imgs.at(startPos++));
                x += i->w();
            } break;
        };
    }
    return startPos;
}

// splitText
std::list<Line> splitText(const std::string& text, SharedFont font, int maxW) {
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

        size_t len = pos2 - pos1;
        std::string str = text.substr(pos1, len);
        int count = 0, width = 0;
        TTF_MeasureUTF8(font.get(), str.c_str(), lines.back().getSpace(maxW),
                        &width, &count);

        if (count == len) {  // Fit entire text onto line
            lines.back().addElement(std::make_unique<Text>(pos1, len, width));
        } else {  // Break up text
            // Find last space, if any
            size_t lastSpace = str.find_last_of(' ');
            if (lastSpace != std::string::npos) {  // Break into words
                int textW;
                TTF_SizeText(font.get(), str.substr(0, lastSpace).c_str(),
                             &textW, nullptr);

                lines.back().addElement(
                    std::make_unique<Text>(pos1, pos1 + lastSpace, textW));
                lines.push_back(Line());
                addText(pos1 + lastSpace + 1, pos2);
            } else {  // Won't fit on this line
                // Get the length until the next break
                int wordW = 0;
                size_t space = str.find(' ');
                TTF_SizeUTF8(font.get(), str.substr(0, space).c_str(), &wordW,
                             nullptr);
                if (wordW <= maxW) {  // It will fit on the next line
                    lines.push_back(Line());
                    addText(pos1, pos2);
                } else {  // It is bigger than one line, split across
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
                   const std::vector<std::string>& imgs, Rect::Align alignX,
                   Rect::Align alignY) {
    SharedFont font = AssetManager::getFont({-1, 25, "|"});
    mLines = splitText(text, font, rect.W());

    int lineH = TTF_FontHeight(font.get());
    TextureBuilder tex(rect.W(), lineH * mLines.size());

    Rect r = Rect::getMinRect(tex.getTexture().get(), rect.w(), rect.h());
    DimensionsF scale{r.w() / rect.w(), r.h() / lineH / mLines.size()};
    r.setPos(rect, alignX, alignY);

    Rect lineR(0, 0, rect.W(), lineH);
    size_t p = 0;
    for (auto& line : mLines) {
        p = line.draw(tex, lineR, {r.x(), r.y()}, scale, font, text, imgs, p,
                      alignX, alignY);
        lineR.move(0, lineR.h());
    }

    addComponent<PositionComponent>(r);
    addComponent<ElevationComponent>(10);
    addComponent<SpriteComponent>(tex.getTexture());
    addComponent<RenderService>();
}
