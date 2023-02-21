#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H

#include <Components/ComponentManager.h>
#include <Entities/Entity.h>
#include <Framework/RenderSystem/Services.h>
#include <Framework/RenderSystem/SpriteComponent.h>
#include <Utils/Colors.h>

#include <list>

// Forward Declaration
struct TextData;

struct Text {
    void draw(TextureBuilder& tex, Rect rect, const SharedFont& font,
              const std::string& text);

    size_t start, len;
    int w;
};

class ImageEntity : public Entities::Entity {
   public:
    void setRect(const Rect& r);
    void setImg(const SpriteData& sprite);

   private:
    void init();
};

typedef std::unique_ptr<ImageEntity> ImageEntityPtr;

struct Line {
   public:
    friend class TextData;
    enum Type : uint8_t {
        TEXT = 0,
        IMAGE,
    };

    bool empty() const;
    int w() const;
    int getSpace(int maxW) const;
    size_t numImgs() const;

    void addText(const Text& text);
    void addImage(int lineH);

   private:
    int mW = 0, mImgCnt = 0;
    std::vector<Type> mTypes;
    std::vector<Text> mText;
};

std::vector<Line> splitText(const std::string& text, SharedFont font, int maxW);

struct TextData : public Components::Component, public Entities::Entity {
   public:
    TextData();

    void setText(const std::string& text);
    void setImages(const SpriteVector& imgs);
    void setRect(const Rect& rect);
    void setFont(SharedFont font);
    void setAlign(Rect::Align a);
    void setAlign(Rect::Align aX, Rect::Align aY);

   private:
    friend class TextComponent;

    void draw();

    bool updateText = true, updateImgs = true;
    SharedFont mFont;
    std::string mText;
    Rect::Align mAlignX = Rect::Align::CENTER, mAlignY = Rect::Align::CENTER;
    Rect mRect;
    std::vector<Line> mLines;
    SpriteVector mImgs;
    std::vector<ImageEntityPtr> mImgEntities;
};

class TextComponent : public Components::ComponentManager<TextData> {
   public:
   private:
    void manager_init();
};

#endif