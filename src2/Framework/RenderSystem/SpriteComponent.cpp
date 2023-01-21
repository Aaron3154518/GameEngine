#include "SpriteComponent.h"

// SpriteComponent
SpriteComponent::SpriteComponent() {}
SpriteComponent::SpriteComponent(SharedTexture tex) : mTex(tex) {}
SpriteComponent::SpriteComponent(const std::string& file)
    : SpriteComponent(AssetManager::getTexture(file)) {}
