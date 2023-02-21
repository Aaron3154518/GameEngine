#include "SpriteComponent.h"

// SpriteData
SpriteData::SpriteData() {}
SpriteData::SpriteData(SharedTexture tex, uint8_t frames, uint32_t delayMs) {
    setTexture(tex);
    setAnimation(frames, delayMs);
}
SpriteData::SpriteData(const std::string& file, uint8_t frames,
                       uint32_t delayMs)
    : SpriteData(AssetManager::getTexture(file), frames, delayMs) {}

void SpriteData::setTexture(SharedTexture tex) { mTex = tex; }

void SpriteData::setTexture(const std::string& file) {
    setTexture(AssetManager::getTexture(file));
}

void SpriteData::setAnimation(uint8_t frames, uint32_t delayMs) {
    seekFrame(0);
    uint8_t prevFrameCnt = mFrameCnt;
    mFrameCnt = frames < 1 ? 1 : frames;
    mDelayMs = delayMs < 1 ? 1 : delayMs;

    float frac = (float)prevFrameCnt / mFrameCnt;
    mTex.mArea = Rect(mTex.mArea.x() * frac, mTex.mArea.y() * frac,
                      mTex.mArea.w() * frac, mTex.mArea.h());
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
