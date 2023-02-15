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

typedef std::unique_ptr<Text> TextPtr;

struct Image : public Entities::Entity {
   public:
    Image(int lineH);

    int w() const;

    void draw(Rect rect, const std::string& text);

   private:
    int mW;
};

typedef std::unique_ptr<Image> ImagePtr;

struct Line {
   public:
    enum Type : uint8_t {
        TEXT = 0,
        IMAGE,
    };

    bool empty() const;
    int w() const;
    int getSpace(int maxW) const;

    void addElement(TextPtr text);
    void addElement(ImagePtr img);

    size_t draw(TextureBuilder& tex, Rect rect, SDL_FPoint off,
                DimensionsF scale, const SharedFont& font,
                const std::string& text, const std::vector<std::string>& imgs,
                size_t startPos);

   private:
    int mW = 0;
    std::list<Type> mTypes;
    std::list<TextPtr> mText;
    std::list<ImagePtr> mImgs;
};

typedef std::unique_ptr<Line> LinePtr;

std::list<Line> splitText(const std::string& text, SharedFont font, int maxW);

struct TextData : public Components::Component, public Entities::Entity {
   public:
    TextData(const std::string& text, const std::vector<std::string>& imgs,
             const Rect& rect, SharedFont font,
             Rect::Align alignX = Rect::Align::CENTER,
             Rect::Align alignY = Rect::Align::CENTER);

   private:
    std::list<Line> mLines;
};

class TextComponent : public Components::ComponentManager<TextData> {};

#endif