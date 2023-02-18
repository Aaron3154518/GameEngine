#include "Camera.h"

// Camera
Camera& Camera::Get() { return GameObjects::Get<Camera>(); }

Rect& Camera::GetRect() {
    return Get().getComponent<PositionComponent>().get();
}

Rect Camera::GetRelativeRect(Rect r) {
    Rect& camera = GetRect();
    Dimensions screenDim = RenderSystem::getWindowSize();
    float wFrac = screenDim.w / camera.w(), hFrac = screenDim.h / camera.h();
    SDL_FPoint c = r.getPos(Rect::Align::CENTER);
    r.setDim(r.w() * wFrac, r.h() * hFrac);
    r.setPos((c.x - camera.x()) * wFrac, (c.y - camera.y()) * hFrac,
             Rect::Align::CENTER);
    return r;
}

void Camera::init() {
    Dimensions d = RenderSystem::getWindowSize();
    addComponent<PositionComponent>(Rect(100, 0, d.w, d.h));
}
