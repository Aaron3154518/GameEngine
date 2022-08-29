#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <RenderSystem/AssetManager.h>
#include <RenderSystem/Renderer.h>
#include <RenderSystem/TextureBuilder.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <functional>
#include <list>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

struct Element {
    Element(int w = 0);
    virtual ~Element() = default;

    virtual void draw(TextureBuilder& tex, Rect rect, Rect::Align align,
                      TTF_Font* font, std::string& text, SDL_Color color) const;

    int mW;
};

typedef std::unique_ptr<Element> ElementPtr;

struct Text : public Element {
    Text(int startPos, int len, int w);

    void draw(TextureBuilder& tex, Rect rect, Rect::Align align, TTF_Font* font,
              std::string& text, SDL_Color color) const;

    int mStartPos, mLen;
};

typedef std::unique_ptr<Text> TextPtr;

struct Image : public Element {
    Image(const std::string& img, int lineH);

    void draw(TextureBuilder& tex, Rect rect, Rect::Align align, TTF_Font* font,
              std::string& text, SDL_Color color) const;

    std::string mImg;
};

typedef std::unique_ptr<Image> ImagePtr;

struct Line : public Element {
    bool empty() const;
    int getSpace(int maxW) const;

    void addElement(ElementPtr e);

    void draw(TextureBuilder& tex, Rect rect, Rect::Align align, TTF_Font* font,
              std::string& text, SDL_Color color) const;

    std::list<ElementPtr> mElements;
};

typedef std::unique_ptr<Line> LinePtr;

std::unique_ptr<std::list<LinePtr>> splitText2(std::string& text,
                                               SharedFont font, int maxW);

#endif
