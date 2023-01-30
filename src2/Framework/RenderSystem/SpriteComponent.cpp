#include "SpriteComponent.h"

// SpriteComponent
SpriteComponent::SpriteComponent() {}
SpriteComponent::SpriteComponent(SharedTexture tex) : mTex(tex) {}
SpriteComponent::SpriteComponent(const std::string& file, uint8_t frames,
                                 uint32_t delayMs)
    : SpriteComponent(AssetManager::getTexture(file)) {
    mFrameCnt = frames < 1 ? 1 : frames;
    mDelayMs = delayMs < 1 ? 1 : delayMs;
}

Dimensions SpriteComponent::getTextureDim() const {
    Dimensions d = AssetManager::getTextureSize(mTex.get());
    d.w /= mFrameCnt;
    return d;
}

// SpriteComponentManager
void SpriteComponentManager::manager_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) { onUpdate(m.data); },
        EventSystem::Update);
}

void SpriteComponentManager::onUpdate(Time dt) {
    for (auto& comp : *this) {
        comp.mTimer += (float)dt.ms() / comp.mDelayMs;
        comp.mFrame =
            (comp.mFrame + (unsigned int)(comp.mTimer)) % comp.mFrameCnt;
        comp.mTimer = fmodf(comp.mTimer, 1.0f);
    }
}
