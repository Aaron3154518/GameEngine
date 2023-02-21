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

int Line::numImgs() const { return mImgs.size(); }

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
TextData::TextData() { init(); }

void TextData::setText(const std::string& text) {
    mText = text;
    updateText = true;
}

void TextData::setImages(const SpriteVector& imgs) {
    mImgs = imgs;
    updateImgs = true;
}

void TextData::setRect(const Rect& rect) {
    mRect = rect;
    updateText = true;
}

void TextData::setFont(SharedFont font) {
    mFont = font;
    updateText = true;
}

void TextData::setAlign(Rect::Align a) { setAlign(a, a); }

void TextData::setAlign(Rect::Align aX, Rect::Align aY) {
    mAlignX = aX;
    mAlignY = aY;
    updateText = true;
}

void TextData::init() {
    subscribeTo<RenderService::Message>(
        [this](const RenderService::Message& m) { draw(); },
        RenderService::PreRender);

    addComponent<PositionComponent>(Rect());
    addComponent<ElevationComponent>(10);
    addComponent<SpriteComponent>();
    addComponent<RenderService>();
}

void TextData::draw() {
    if (updateText) {
        int lineH = TTF_FontHeight(mFont.get());
        mLines = splitText(mText, mFont, mRect.W());
        TextureBuilder tex(mRect.W(), lineH * mLines.size());

        Rect r = Rect(0, 0, mRect.W(), lineH * mLines.size());
        DimensionsF scale{r.w() / mRect.w(), r.h() / lineH / mLines.size()};
        r.setPos(mRect, mAlignX, mAlignY);

        int numImgs = 0;
        for (auto& line : mLines) {
            numImgs += line.numImgs();
        }
        size_t currSize = mImgEntities.size();
        mImgEntities.resize(numImgs);
        for (size_t i = currSize; i < mImgEntities.size(); i++) {
            mImgEntities.at(i) = GameObjects::New<ImageEntity>();
        }
        auto imgEntIt = mImgEntities.begin();

        Rect lineR(0, 0, mRect.W(), lineH);
        for (auto& line : mLines) {
            lineR.setWidth(line.w(), mAlignX);
            float x = lineR.x();
            auto textIt = line.mText.begin();
            auto imgIt = line.mImgs.begin();
            for (auto type : line.mTypes) {
                switch (type) {
                    case Line::Type::TEXT: {
                        Text& t = *(textIt++);
                        t.draw(tex, Rect(x, lineR.y(), t.w(), lineR.h()), mFont,
                               mText);
                        x += t.w();
                    } break;
                    case Line::Type::IMAGE: {
                        Image& i = *(imgIt++);
                        (*(imgEntIt++))
                            ->setRect(Rect(x * scale.w + r.x(),
                                           lineR.y() * scale.h + r.y(),
                                           i.w() * scale.h,
                                           lineR.h() * scale.h));
                        x += i.w();
                    } break;
                };
            }
            lineR.move(0, lineR.h());
        }

        getComponent<PositionComponent>().set(r);
        getComponent<SpriteComponent>().setTexture(tex.getTexture());

        updateText = false;
    }
    if (updateImgs) {
        auto imgIt = mImgs.begin();
        for (auto it = mImgEntities.begin(); it != mImgEntities.end(); ++it) {
            if (imgIt != mImgs.end()) {
                (*it)->setImg(*imgIt);
            } else {
                (*it)->setImg(SpriteData());
            }
        }

        updateImgs = false;
    }
}
