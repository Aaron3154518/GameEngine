#include "SpriteComponent.h"

// SpriteData
SpriteData::SpriteData() {}
SpriteData::SpriteData(SharedTexture tex) : mTex(tex) {}
SpriteData::SpriteData(const std::string& file, uint8_t frames,
                       uint32_t delayMs)
    : SpriteData(AssetManager::getTexture(file)) {
    mFrameCnt = frames < 1 ? 1 : frames;
    mDelayMs = delayMs < 1 ? 1 : delayMs;
}

Dimensions SpriteData::getTextureDim() const {
    Dimensions d = AssetManager::getTextureSize(mTex.get());
    d.w /= mFrameCnt;
    return d;
}

// SpriteComponent
void SpriteComponent::manager_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) { onUpdate(m.data); },
        EventSystem::Update);
}

void SpriteComponent::onUpdate(Time dt) {
    for (auto& comp : *this) {
        comp.mTimer += (float)dt.ms() / comp.mDelayMs;
        comp.mFrame =
            (comp.mFrame + (unsigned int)(comp.mTimer)) % comp.mFrameCnt;
        comp.mTimer = fmodf(comp.mTimer, 1.0f);
    }
}
