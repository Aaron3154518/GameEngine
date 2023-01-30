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
    SpriteComponent(const std::string& file);

    SharedTexture mTex;
    Rect mBounds, mArea = Rect(0, 0, 1, 1);
};

typedef Components::ComponentManager<SpriteComponent> SpriteComponentManager;

struct AnimationComponent : public Components::Component {
    AnimationComponent(uint8_t frames, uint32_t delayMs);

    uint8_t mFrameCnt = 1;
    uint32_t mDelayMs = 0;
};

typedef Components::ComponentManager<AnimationComponent>
    AnimationComponentManager;

class AnimationService : public Services::Service {
   private:
    void onSubscribe(const Entities::UUID& eId);
    void onUnsubscribe(const Entities::UUID& eId);

    void onUpdate(Time dt);

    void service_init();

    std::unordered_map<Entities::UUID, float> mTimers;
};

#endif
