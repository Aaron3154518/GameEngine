#ifndef SPRITE_COMPONENT
#define SPRITE_COMPONENT

#include <Components/Component.h>
#include <Components/ComponentManager.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/RenderSystem/AssetManager.h>
#include <Framework/RenderSystem/RenderData.h>
#include <Services/Service.h>
#include <Utils/Pointers.h>
#include <Utils/Rect.h>

#include <cmath>
#include <unordered_map>

struct SpriteData : public Components::Component {
    friend class SpriteComponent;

   public:
    SpriteData();
    SpriteData(SharedTexture tex, uint8_t frames = 1, uint32_t delayMs = 100);
    SpriteData(const std::string& file, uint8_t frames = 1,
               uint32_t delayMs = 100);

    void setTexture(SharedTexture tex);
    void setTexture(const std::string& file);
    void setAnimation(uint8_t frames = 1, uint32_t delayMs = 100);

    void seekFrame(uint8_t frame);
    uint8_t frame() const;
    uint8_t frameCount() const;

    void draw(TextureBuilder& tex, const Rect& r);

   private:
    uint8_t mFrameCnt = 1, mFrame = 0;
    uint32_t mDelayMs = 0;
    float mTimer = 0;
    RenderData mTex;
};

class SpriteComponent : public Components::ComponentManager<SpriteData> {
   private:
    void manager_init();

    void onUpdate(Time dt);
};

typedef SpriteComponent::ComponentPtr SpritePtr;
typedef std::vector<SpriteData> SpriteVector;

#endif
