#include "TextComponent.h"

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

// Text
void Text::draw(TextureBuilder& tex, Rect rect, const SharedFont& font,
                const std::string& text) {
    Surface textSurf = makeSurface();
    std::string str = text.substr(start, len);
    textSurf = makeSurface(
        TTF_RenderText_Blended(font.get(), str.c_str(), Colors::Black));

    RenderData rd(makeSharedTexture(
        SDL_CreateTextureFromSurface(Renderer::get(), textSurf.get())));
    rd.mRect = rd.getMinRect(rect);
    rd.mRect.setPos(rect, Rect::Align::CENTER);
    tex.draw(rd);
}

// Line
bool Line::empty() const { return mTypes.empty(); }

int Line::w() const { return mW; }

int Line::getSpace(int maxW) const { return maxW - mW; }

size_t Line::numImgs() const { return mImgCnt; }

void Line::addText(const Text& text) {
    mW += text.w;
    mText.push_back(text);
    mTypes.push_back(Type::TEXT);
}
void Line::addImage(int lineH) {
    mW += lineH;
    mImgCnt++;
    mTypes.push_back(Type::IMAGE);
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

    std::function<void(size_t, size_t)> addText =
        [&lines, &font, &text, &addText, maxW, spaceW](size_t pos1,
                                                       size_t pos2) {
            if (pos1 >= pos2) {
                return;
            }

            size_t len = pos2 - pos1;
            std::string str = text.substr(pos1, len);
            int count = 0, width = 0;
            TTF_MeasureUTF8(font.get(), str.c_str(),
                            lines.back().getSpace(maxW), &width, &count);

            if ((size_t)count == len) {  // Fit entire text onto line
                lines.back().addText({pos1, len, width});
            } else {  // Break up text
                // Find last space, if any
                size_t lastSpace = str.find_last_of(' ');
                if (lastSpace != std::string::npos) {  // Break into words
                    int textW;
                    TTF_SizeText(font.get(), str.substr(0, lastSpace).c_str(),
                                 &textW, nullptr);

                    lines.back().addText({pos1, pos1 + lastSpace, textW});
                    lines.push_back(Line());
                    addText(pos1 + lastSpace + 1, pos2);
                } else {  // Won't fit on this line
                    // Get the length until the next break
                    int wordW = 0;
                    size_t space = str.find(' ');
                    TTF_SizeUTF8(font.get(), str.substr(0, space).c_str(),
                                 &wordW, nullptr);
                    if (wordW <= maxW) {  // It will fit on the next line
                        lines.push_back(Line());
                        addText(pos1, pos2);
                    } else {  // It is bigger than one line, split across
                        // multiple lines
                        lines.back().addText({pos1, (size_t)count, width});
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
                        lines.back().addImage(lineH);
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
TextData::TextData() {
    addComponent<PositionComponent>(Rect());
    addComponent<ElevationComponent>(10);
    addComponent<SpriteComponent>();
    addComponent<RenderService>();
}

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

void TextData::draw() {
    if (updateText) {
        int lineH = TTF_FontHeight(mFont.get());
        mLines = splitText(mText, mFont, mRect.W());
        TextureBuilder tex(mRect.W(), lineH * mLines.size());

        Rect r = Rect(0, 0, mRect.W(), lineH * mLines.size());
        DimensionsF scale{r.w() / mRect.w(), r.h() / lineH / mLines.size()};
        r.setPos(mRect, mAlignX, mAlignY);

        size_t numImgs = 0;
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
        float imgW = lineR.h() * scale.h;
        for (auto& line : mLines) {
            lineR.setWidth(line.w(), mAlignX);
            float x = lineR.x();
            auto textIt = line.mText.begin();
            for (auto type : line.mTypes) {
                switch (type) {
                    case Line::Type::TEXT: {
                        Text& t = *(textIt++);
                        t.draw(tex, Rect(x, lineR.y(), t.w, lineR.h()), mFont,
                               mText);
                        x += t.w;
                    } break;
                    case Line::Type::IMAGE: {
                        (*(imgEntIt++))
                            ->setRect(Rect(x * scale.w + r.x(),
                                           lineR.y() * scale.h + r.y(), imgW,
                                           imgW));
                        x += imgW;
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

// TextComponent
void TextComponent::manager_init() {
    subscribeTo<RenderService::Message>(
        [this](const RenderService::Message& m) { forEach(&TextData::draw); },
        RenderService::PreRender);
}
