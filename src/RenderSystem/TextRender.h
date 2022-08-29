#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <RenderSystem/AssetManager.h>
#include <RenderSystem/RenderTypes.h>
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

// To render text
struct TextData {
    SDL_Color color = BLACK;
    SDL_Color bkgrnd = TRANSPARENT;

    bool autoFit = true;
    Rect::Align align = Rect::Align::CENTER;

    SharedFont font = makeSharedFont();

    TextData& setText(const std::string& text);
    TextData& setText(const std::string& text, int w,
                      const std::initializer_list<RenderData>& imgs = {});
    TextData& setTextImgs(const std::initializer_list<RenderData>& imgs);

    // Functions to render text to a texture
    SharedTexture renderText() const;
    SharedTexture renderTextLine() const;
    SharedTexture renderTextWrapped() const;

   private:
    std::string mText = "";
    // For wrapping text
    // w > 0 will wrap text
    int mW = 0;
    std::vector<RenderData> mImgs;
};

struct Element {
    Element(int w = 0);
    virtual ~Element() = default;

    virtual void draw(TextureBuilder& tex, Rect rect, const TextData& td,
                      std::string& text);

    int mW;
};

typedef std::unique_ptr<Element> ElementPtr;

struct Text : public Element {
   public:
    Text(int startPos, int len, int w);

    void draw(TextureBuilder& tex, Rect rect, const TextData& td,
              std::string& text);

   private:
    int mStartPos, mLen;
};

typedef std::unique_ptr<Text> TextPtr;

struct Image : public Element {
   public:
    Image(const RenderData& img, int lineH);

    void draw(TextureBuilder& tex, Rect rect, const TextData& td,
              std::string& text);

   private:
    RenderData mImg;
};

typedef std::unique_ptr<Image> ImagePtr;

struct Line : public Element {
   public:
    bool empty() const;
    int getSpace(int maxW) const;

    void addElement(ElementPtr e);

    void draw(TextureBuilder& tex, Rect rect, const TextData& td,
              std::string& text);

   private:
    std::list<ElementPtr> mElements;
};

typedef std::unique_ptr<Line> LinePtr;

std::unique_ptr<std::list<LinePtr>> splitText(
    std::string& text, SharedFont font, int maxW,
    const std::vector<RenderData>& imgs = {});

#endif
