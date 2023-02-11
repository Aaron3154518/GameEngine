#include "SpriteComponent.h"

// SpriteData
SpriteData::SpriteData() {}
SpriteData::SpriteData(SharedTexture tex) : mTex(tex) {}
SpriteData::SpriteData(const std::string& file, uint8_t frames,
                       uint32_t delayMs)
    : SpriteData(AssetManager::getTexture(file)) {
    mFrameCnt = frames < 1 ? 1 : frames;
    mDelayMs = delayMs < 1 ? 1 : delayMs;
    mTex.mArea = Rect(0, 0, 1.0f / mFrameCnt, 1);
}

void SpriteData::seekFrame(uint8_t frame) {
    mFrame = std::min(frame, (uint8_t)(mFrameCnt - 1));
    mTex.mArea.setPosX(
        fmodf(mTex.mArea.x(), mTex.mArea.w()) + mFrame * mTex.mArea.w(),
        Rect::Align::TOP_LEFT);
}

uint8_t SpriteData::frame() const { return mFrame; }
uint8_t SpriteData::frameCount() const { return mFrameCnt; }

void SpriteData::draw(TextureBuilder& tex, const Rect& r) {
    mTex.mRect = r;
    tex.draw(mTex);
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
        comp.seekFrame((comp.mFrame + (unsigned int)(comp.mTimer)) %
                       comp.mFrameCnt);
        comp.mTimer = fmodf(comp.mTimer, 1.0f);
    }
}
