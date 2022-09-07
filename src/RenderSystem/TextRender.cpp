#include "TextRender.h"

// Text
Text::Text(int startPos, int len, int w)
    : mW(w), mStartPos(startPos), mLen(len) {}

int Text::w() const { return mW; }

void Text::draw(TextureBuilder& tex, Rect rect, const TextData& td,
                std::string& text) {
    Surface textSurf = makeSurface();
    size_t endPos = mStartPos + mLen;
    const auto& tData = td.data();
    if (endPos != text.size()) {
        char tmp = text.at(endPos);
        text.replace(endPos, 1, 1, '\0');
        textSurf = makeSurface(TTF_RenderText_Blended(
            tData.mFont.get(), text.c_str() + mStartPos, tData.mColor));
        text.replace(endPos, 1, 1, tmp);
    } else {
        textSurf = makeSurface(TTF_RenderText_Blended(
            tData.mFont.get(), text.c_str() + mStartPos, tData.mColor));
    }

    SharedTexture textTex = makeSharedTexture(
        SDL_CreateTextureFromSurface(Renderer::get(), textSurf.get()));
    RenderData rd = RenderData()
                        .set(textTex)
                        .setFit(RenderData::FitMode::Texture)
                        .setFitAlign(tData.mAlign, Rect::Align::CENTER)
                        .setDest(rect);
    tex.draw(rd);
}

// Image
Image::Image(int lineH) : mW(lineH) {}

int Image::w() const { return mW; }

void Image::draw(TextureBuilder& tex, Rect rect, RenderDataCWPtr data) {
    auto rData = data.lock();
    if (rData) {
        RenderData rd = RenderData(*rData).setDest(rect);
        tex.draw(rd);
    }
}

// Line
bool Line::empty() const { return mTypes.empty(); }

int Line::w() const { return mW; }

int Line::getSpace(int maxW) const { return maxW - mW; }

void Line::addElement(const Text& text) {
    mText.push_back(text);
    mTypes.push_back(Type::TEXT);
    mW += text.w();
}
void Line::addElement(const Image& img) {
    mImgs.push_back(img);
    mTypes.push_back(Type::IMAGE);
    mW += img.w();
}

void Line::drawText(TextureBuilder& tex, Rect rect, const TextData& td,
                    std::string& text) {
    Rect lineRect(0, 0, mW, rect.h());
    lineRect.setPos(rect, td.data().mAlign, Rect::Align::CENTER);
    auto textIt = mText.begin();
    auto imgIt = mImgs.begin();
    for (auto type : mTypes) {
        switch (type) {
            case Type::TEXT: {
                Text& t = *(textIt++);
                lineRect.setDim(t.w(), rect.h(), Rect::Align::TOP_LEFT,
                                Rect::Align::CENTER);
                t.draw(tex, lineRect, td, text);
                lineRect.move(t.w(), 0);
            } break;
            case Type::IMAGE:
                lineRect.move((imgIt++)->w(), 0);
                break;
        };
    }
}
size_t Line::drawImages(TextureBuilder& tex, Rect rect, const TextData& td,
                        const std::vector<RenderDataCWPtr>& imgs,
                        size_t startPos) {
    if (mImgs.size() > imgs.size() - startPos) {
        std::cerr << "Line::drawImages(): Expected " << mImgs.size()
                  << " images but received " << (imgs.size() - startPos)
                  << std::endl;
        return startPos;
    }
    Rect lineRect(0, 0, mW, rect.h());
    lineRect.setPos(rect, td.data().mAlign, Rect::Align::CENTER);
    auto textIt = mText.begin();
    auto imgIt = mImgs.begin();
    for (auto type : mTypes) {
        switch (type) {
            case Type::TEXT:
                lineRect.move((textIt++)->w(), 0);
                break;
            case Type::IMAGE: {
                Image& i = *(imgIt++);
                lineRect.setDim(i.w(), rect.h(), Rect::Align::TOP_LEFT,
                                Rect::Align::CENTER);
                auto rectShape = RectShape(td.data().mBkgrnd).set(lineRect);
                tex.draw(rectShape);
                i.draw(tex, lineRect, imgs.at(startPos++));
                lineRect.move(i.w(), 0);
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

    std::function<void(size_t, size_t)> addText =
        [&lines, &font, &text, &addText, maxW, spaceW](size_t pos1,
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
                lines.back().addElement(Text(pos1, len, width));
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

                    auto ptr = Text(pos1, idx, textW);
                    lines.back().addElement(std::move(ptr));
                    lines.push_back(Line());
                    addText(pos1 + idx + 1, pos2);
                } else {  // Won't fit on this line
                    // Get the length until the next break
                    int wordW = 0;
                    auto spaceIt = std::find(text.begin() + pos1,
                                             text.begin() + pos2, ' ');
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
                        lines.back().addElement(Text(pos1, count, width));
                        lines.push_back(Line());
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
SharedTexture TextData::get() {
    if (!mData.mFont) {
        std::cerr << "TextData::get(): Could not load font" << std::endl;
        mTex = makeSharedTexture();
        return mTex;
    }

    bool wrap = mData.mW > 0;
    bool forceImgUpdate = false;

    switch (mUpdateStatus) {
        case Update::SPLIT: {
            if (wrap) {
                std::string text = mData.mText;
                mLines = splitText(text, mData.mFont, mData.mW);
                if (mLines.empty()) {
                    std::cerr << "TextData::get(): No lines" << std::endl;
                    mTex = makeSharedTexture();
                    break;
                }
            } else {
                mLines.clear();
            }
        }
        case Update::DRAW: {
            if (wrap) {
                int maxLineW = 0;
                if (mData.mAutoFit) {
                    for (auto& line : mLines) {
                        if (line.w() > maxLineW) {
                            maxLineW = line.w();
                        }
                    }
                } else {
                    maxLineW = mData.mW;
                }

                int lineH = TTF_FontHeight(mData.mFont.get());
                int h = lineH * mLines.size();
                TextureBuilder tex(maxLineW, h, mData.mBkgrnd);
                Rect lineR(0, 0, maxLineW, lineH);
                std::string text = mData.mText;
                for (auto& line : mLines) {
                    if (!line.empty()) {
                        line.drawText(tex, lineR, *this, text);
                    }
                    lineR.move(0, lineH);
                }
                mTex = tex.getTexture();
            } else {
                if (mData.mText.empty()) {
                    mTex = makeSharedTexture();
                    break;
                }
                Surface surface = makeSurface();
                if (mData.mBkgrnd == TRANSPARENT) {
                    surface.reset(TTF_RenderText_Blended(
                        mData.mFont.get(), mData.mText.c_str(), mData.mColor));
                } else {
                    surface.reset(TTF_RenderText_Shaded(
                        mData.mFont.get(), mData.mText.c_str(), mData.mColor,
                        mData.mBkgrnd));
                }
                if (!surface) {
                    std::cerr << "TextData::get(): Unable to render text"
                              << std::endl;
                    mTex = makeSharedTexture();
                    break;
                }
                mTex = makeSharedTexture(SDL_CreateTextureFromSurface(
                    Renderer::get(), surface.get()));
            }
            forceImgUpdate = true;
        }
        case Update::IMGS: {
            if (!wrap || !mTex) {
                break;
            }

            bool update = false;
            for (size_t i = 0; i < mImgs.size(); i++) {
                auto ptr = mImgs.at(i).lock();
                Uint32 lastUpdated = ptr ? ptr->getLastUpdated() : 0;
                if (mImgVersions.at(i) != lastUpdated) {
                    mImgVersions.at(i) = lastUpdated;
                    update = true;
                }
            }
            // We can only skip updating if we didn't do anything else
            if (!forceImgUpdate && !update) {
                break;
            }

            TextureBuilder tex(mTex, false);
            SDL_Point dim = tex.getTextureSize();
            int lineH = dim.y / mLines.size();
            Rect lineR(0, 0, dim.x, lineH);
            size_t imgStartPos = 0;
            for (auto& line : mLines) {
                if (!line.empty()) {
                    imgStartPos =
                        line.drawImages(tex, lineR, *this, mImgs, imgStartPos);
                }
                lineR.move(0, lineH);
            }
            break;
        }
    };
    mUpdateStatus = Update::IMGS;
    return mTex;
}

const TextDataData& TextData::data() const { return mData; }

TextData& TextData::setColor(SDL_Color color) {
    mData.mColor = color;
    setUpdateStatus(Update::DRAW);
    return *this;
}
TextData& TextData::setBackground(SDL_Color bkgrnd) {
    mData.mBkgrnd = bkgrnd;
    setUpdateStatus(Update::DRAW);
    return *this;
}

TextData& TextData::setAutoFit(bool fit) {
    mData.mAutoFit = fit;
    setUpdateStatus(Update::DRAW);
    return *this;
}
TextData& TextData::setAlign(Rect::Align a) {
    mData.mAlign = a;
    setUpdateStatus(Update::DRAW);
    return *this;
}

TextData& TextData::setFont(const FontData& font) {
    return setFont(AssetManager::getFont(font));
}
TextData& TextData::setFont(SharedFont font) {
    mData.mFont = font;
    setUpdateStatus(Update::SPLIT);
    return *this;
}

TextData& TextData::setText(const std::string& text, int w) {
    mData.mText = text;
    mData.mW = w;
    setUpdateStatus(Update::SPLIT);
    return mData.mW <= 0 ? setImgs({}) : *this;
}
TextData& TextData::setImgs(const std::vector<RenderDataCWPtr>& imgs) {
    mImgs = imgs;
    mImgVersions.resize(mImgs.size(), 0);
    return *this;
}

void TextData::setUpdateStatus(Update u) {
    if (u > mUpdateStatus) {
        mUpdateStatus = u;
    }
}
