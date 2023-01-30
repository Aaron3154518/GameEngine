#include "SpriteComponent.h"

// SpriteComponent
SpriteComponent::SpriteComponent() {}
SpriteComponent::SpriteComponent(SharedTexture tex) : mTex(tex) {}
SpriteComponent::SpriteComponent(const std::string& file)
    : SpriteComponent(AssetManager::getTexture(file)) {}

// AnimationComponent
AnimationComponent::AnimationComponent(uint8_t frames, uint32_t delayMs)
    : mFrameCnt(frames < 1 ? 1 : frames), mDelayMs(delayMs < 1 ? 1 : delayMs) {}

// AnimationService
void AnimationService::service_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) { onUpdate(m.data); },
        EventSystem::Update);
}

void AnimationService::onSubscribe(const Entities::UUID& eId) {
    mTimers[eId] = 0;
    auto& area = GameObjects::Get<SpriteComponentManager>()[eId].mArea;
    auto& anim = GameObjects::Get<AnimationComponentManager>()[eId];
    area.setPosX(area.x() / anim.mFrameCnt, Rect::Align::TOP_LEFT);
    area.setWidth(area.w() / anim.mFrameCnt, Rect::Align::TOP_LEFT);
}

void AnimationService::onUnsubscribe(const Entities::UUID& eId) {
    auto it = mTimers.find(eId);
    if (it != mTimers.end()) {
        mTimers.erase(it);
    }
}

void AnimationService::onUpdate(Time dt) {
    auto& am = GameObjects::Get<AnimationComponentManager>();
    auto& sm = GameObjects::Get<SpriteComponentManager>();
    for (auto& pair : mTimers) {
        auto& anim = am[pair.first];
        pair.second += (float)dt.ms() / anim.mDelayMs;
        float frameSkip = std::floor(pair.second);
        pair.second = fmodf(pair.second, 1.0f);
        Rect& area = sm[pair.first].mArea;
        area.setPosX(fmodf(area.x() + frameSkip / anim.mFrameCnt, 1.0f),
                     Rect::Align::TOP_LEFT);
    }
}
