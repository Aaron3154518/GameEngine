#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H

#include <RenderSystem/AssetManager.h>
#include <RenderSystem/RenderTypes.h>
#include <RenderSystem/Renderer.h>
#include <RenderSystem/Shapes.h>
#include <RenderSystem/TextureBuilder.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <functional>
#include <list>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// Forward Declaration
struct TextData;

struct Text {
   public:
    Text(int startPos, int len, int w);

    int w() const;

    void draw(TextureBuilder& tex, Rect rect, const TextData& td,
              std::string& text);

   private:
    int mStartPos, mLen, mW;
};

struct Image {
   public:
    Image(int lineH);

    int w() const;

    void draw(TextureBuilder& tex, Rect rect, RenderData data);

   private:
    int mW;
};

struct Line {
   public:
    enum Type : uint8_t {
        TEXT = 0,
        IMAGE,
    };

    bool empty() const;
    int w() const;
    int getSpace(int maxW) const;

    void addElement(const Text& text);
    void addElement(const Image& img);

    void drawText(TextureBuilder& tex, Rect rect, const TextData& td,
                  std::string& text);
    size_t drawImages(TextureBuilder& tex, Rect rect, const TextData& td,
                      const std::vector<RenderData>& imgs, size_t startPos = 0);

   private:
    int mW = 0;
    std::list<Type> mTypes;
    std::list<Text> mText;
    std::list<Image> mImgs;
};

typedef std::unique_ptr<Line> LinePtr;

std::list<Line> splitText(std::string& text, SharedFont font, int maxW);

// To render text
struct TextDataData {
    SDL_Color mColor = BLACK;
    SDL_Color mBkgrnd = TRANSPARENT;

    bool mAutoFit = true;
    Rect::Align mAlign = Rect::Align::CENTER;

    SharedFont mFont = makeSharedFont();
    std::string mText = "";
    // Members for wrapping text
    // w > 0 will wrap text
    int mW = 0;
};

struct TextData {
    SharedTexture get();
    const TextDataData& data() const;

    TextData& setColor(SDL_Color color);
    TextData& setBackground(SDL_Color bkgrnd);

    TextData& setAutoFit(bool fit);
    TextData& setAlign(Rect::Align a);

    TextData& setFont(const FontData& font);
    TextData& setFont(SharedFont font);

    TextData& setText(const std::string& text);
    TextData& setText(const std::string& text, int w,
                      const std::vector<RenderData>& imgs = {});
    TextData& setTextImgs(const std::vector<RenderData>& imgs);

   private:
    enum Update : uint8_t { IMGS = 0, DRAW, SPLIT };
    void setUpdateStatus(Update u);

    Update mUpdateStatus = Update::SPLIT;

    TextDataData mData;

    SharedTexture mTex;
    std::list<Line> mLines;
    std::vector<RenderData> mImgs;
};

#endif
