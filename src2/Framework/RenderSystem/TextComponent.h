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
   public:
    Text(int startPos, int len, int w);

    int w() const;

    void draw(TextureBuilder& tex, Rect rect, const SharedFont& font,
              const std::string& text);

   private:
    int mStartPos, mLen, mW;
};

class ImageEntity : public Entities::Entity {
   private:
    void init();
    void draw(Rect rect, const std::string& text);
};

typedef std::unique_ptr<ImageEntity> ImageEntityPtr;

struct Image {
   public:
    Image(int lineH);

    int w() const;

    ImageEntityPtr draw(Rect rect, const std::string& text);

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

    void draw(TextureBuilder& tex, Rect rect, SDL_FPoint off, DimensionsF scale,
              const SharedFont& font, const std::string& text,
              const std::vector<std::string>& imgs,
              std::vector<ImageEntityPtr>& imgEntities);

   private:
    int mW = 0;
    std::list<Type> mTypes;
    std::list<Text> mText;
    std::list<Image> mImgs;
};

std::vector<Line> splitText(const std::string& text, SharedFont font, int maxW);

struct TextData : public Components::Component, public Entities::Entity {
   public:
    TextData(const std::string& text, const std::vector<std::string>& imgs,
             const Rect& rect, SharedFont font,
             Rect::Align alignX = Rect::Align::CENTER,
             Rect::Align alignY = Rect::Align::CENTER);

    void setText(const std::string& text);
    void setImages(const std::vector<std::string>& imgs);
    void setRect(const Rect& rect);

   private:
    std::vector<ImageEntityPtr> mImgs;
};

class TextComponent : public Components::ComponentManager<TextData> {};

#endif