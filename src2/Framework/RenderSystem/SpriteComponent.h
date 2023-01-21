#ifndef SPRITE_COMPONENT
#define SPRITE_COMPONENT

#include <Components/Component.h>
#include <Components/ComponentManager.h>
#include <Framework/RenderSystem/AssetManager.h>
#include <Utils/Pointers.h>
#include <Utils/Rect.h>

struct SpriteComponent : public Components::Component {
    SpriteComponent();
    SpriteComponent(SharedTexture tex);
    SpriteComponent(const std::string& file);

    SharedTexture mTex;
    Rect mBounds, mArea = Rect(0, 0, 1, 1);
};

typedef Components::ComponentManager<SpriteComponent> SpriteComponentManager;

#endif
