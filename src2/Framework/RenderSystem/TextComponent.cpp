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
    rd.mRect.setPos(rect, Rect::Align::CENTER);
    tex.draw(rd);
}

// ImageEntity
void ImageEntity::init() {
    addComponent<PositionComponent>(Rect());
    addComponent<ElevationComponent>(6);
    addComponent<SpriteComponent>(makeSharedTexture());
    addComponent<RenderService>();
}

void ImageEntity::setRect(const Rect& r) {
    getComponent<PositionComponent>().set(r);
}

void ImageEntity::setImg(const SpriteData& sprite) {
    addComponent<SpriteComponent>(sprite);
}

// Image
Image::Image(int lineH) : mW(lineH) {}

int Image::w() const { return mW; }

ImageEntityPtr Image::draw(Rect rect, const SpriteData& img) {
    ImageEntityPtr ptr = GameObjects::New<ImageEntity>();
    ptr->setRect(rect);
    ptr->setImg(img);
    return ptr;
}

// Line
bool Line::empty() const { return mTypes.empty(); }

int Line::w() const { return mW; }

int Line::getSpace(int maxW) const { return maxW - mW; }

void Line::addElement(const Text& text) {
    mW += text.w();
    mText.push_back(text);
    mTypes.push_back(Type::TEXT);
}
void Line::addElement(const Image& img) {
    mW += img.w();
    mImgs.push_back(img);
    mTypes.push_back(Type::IMAGE);
}

void Line::draw(TextureBuilder& tex, Rect rect, SDL_FPoint off,
                DimensionsF scale, const SharedFont& font,
                const std::string& text, const SpriteVector& imgs,
                std::vector<ImageEntityPtr>& imgEntities) {
    if (mImgs.size() > imgs.size() - imgEntities.size()) {
        std::cerr << "Line::drawImages(): Expected " << mImgs.size()
                  << " images but received "
                  << (imgs.size() - imgEntities.size()) << std::endl;
        return;
    }
    float x = rect.x();
    auto textIt = mText.begin();
    auto imgIt = mImgs.begin();
    for (auto type : mTypes) {
        switch (type) {
            case Type::TEXT: {
                Text& t = *(textIt++);
                t.draw(tex, Rect(x, rect.y(), t.w(), rect.h()), font, text);
                x += t.w();
            } break;
            case Type::IMAGE: {
                Image& i = *(imgIt++);
                imgEntities.push_back(
                    i.draw(Rect(x * scale.w + off.x, rect.y() * scale.h + off.y,
                                i.w() * scale.h, rect.h() * scale.h),
                           imgs.at(imgEntities.size())));
                x += i.w();
            } break;
        };
    }
}

// splitText
std::vector<Line> splitText(const std::string& text, SharedFont font,
                            int maxW) {
    std::vector<Line> lines;
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
            lines.back().addElement(Text(pos1, len, width));
        } else {  // Break up text
            // Find last space, if any
            size_t lastSpace = str.find_last_of(' ');
            if (lastSpace != std::string::npos) {  // Break into words
                int textW;
                TTF_SizeText(font.get(), str.substr(0, lastSpace).c_str(),
                             &textW, nullptr);

                lines.back().addElement(Text(pos1, pos1 + lastSpace, textW));
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
                    lines.back().addElement(Text(pos1, count, width));
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
                        lines.back().addElement(Image(lineH));
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
TextData::TextData(const std::string& text, const SpriteVector& imgs,
                   const Rect& rect, SharedFont font, Rect::Align alignX,
                   Rect::Align alignY) {
    int lineH = TTF_FontHeight(font.get());
    auto lines = splitText(text, font, rect.W());
    TextureBuilder tex(rect.W(), lineH * lines.size());

    Rect r = Rect(0, 0, rect.W(), lineH * lines.size());
    DimensionsF scale{r.w() / rect.w(), r.h() / lineH / lines.size()};
    r.setPos(rect, alignX, alignY);

    mImgs.clear();
    mImgs.reserve(imgs.size());

    Rect lineR(0, 0, rect.W(), lineH);
    for (auto& line : lines) {
        Rect lr = lineR;
        lr.setWidth(line.w(), alignX);
        line.draw(tex, lr, {r.x(), r.y()}, scale, font, text, imgs, mImgs);
        lineR.move(0, lineR.h());
    }

    addComponent<PositionComponent>(r);
    addComponent<ElevationComponent>(10);
    addComponent<SpriteComponent>(tex.getTexture());
    addComponent<RenderService>();
}

void TextData::setText(const std::string& text) {}

void TextData::setImages(const SpriteVector& imgs) {
    if (imgs.size() != mImgs.size()) {
        throw std::runtime_error(
            "TextData::setImages(): Expected " + std::to_string(mImgs.size()) +
            " images but received " + std::to_string(imgs.size()));
    }

    for (size_t i = 0; i < mImgs.size(); i++) {
        mImgs.at(i)->setImg(imgs.at(i));
    }
}

void TextData::setRect(const Rect& rect) {}

void TextData::setFont(SharedFont font) {}
