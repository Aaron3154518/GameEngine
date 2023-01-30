#ifndef SPRITE_COMPONENT
#define SPRITE_COMPONENT

#include <Components/Component.h>
#include <Components/ComponentManager.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/RenderSystem/AssetManager.h>
#include <Services/Service.h>
#include <Utils/Pointers.h>
#include <Utils/Rect.h>

#include <cmath>
#include <unordered_map>

struct SpriteComponent : public Components::Component {
    SpriteComponent();
    SpriteComponent(SharedTexture tex);
    SpriteComponent(const std::string& file, uint8_t frames = 1,
                    uint32_t delayMs = 100);

    Dimensions getTextureDim() const;

    uint8_t mFrameCnt = 1, mFrame = 0;
    uint32_t mDelayMs = 0;
    float mTimer = 0;
    SharedTexture mTex;
    Rect mBounds, mArea = Rect(0, 0, 1, 1);
};

class SpriteComponentManager
    : public Components::ComponentManager<SpriteComponent> {
   private:
    void manager_init();

    void onUpdate(Time dt);
};

#endif
