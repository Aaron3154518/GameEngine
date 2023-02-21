#include "Camera.h"

// Camera
Camera& Camera::Get() { return GameObjects::Get<Camera>(); }

Rect& Camera::getRect() { return getComponent<PositionComponent>().get(); }

Rect Camera::getRelativeRect(Rect r) {
    Rect& camera = getRect();
    Dimensions screenDim = RenderSystem::getWindowSize();
    float wFrac = screenDim.w / camera.w(), hFrac = screenDim.h / camera.h();
    SDL_FPoint c = r.getPos(Rect::Align::CENTER);
    r.setDim(r.w() * wFrac, r.h() * hFrac);
    r.setPos((c.x - camera.x()) * wFrac, (c.y - camera.y()) * hFrac,
             Rect::Align::CENTER);
    return r;
}

void Camera::track(const Entities::UUID& eId, float maxSpeed) {
    mTrackId = eId;
    mSpeed = maxSpeed;
}

void Camera::init() {
    Dimensions d = RenderSystem::getWindowSize();
    addComponent<PositionComponent>(Rect(0, 0, d.w, d.h));

    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) {
            if (mTrackId != Entities::NullId()) {
                SDL_FPoint tPos =
                    GameObjects::Get<PositionComponent>()[mTrackId]
                        .get()
                        .getPos(Rect::Align::CENTER);
                Rect& pos = getComponent<PositionComponent>().get();
                float dx = tPos.x - pos.cX(), dy = tPos.y - pos.cY();
                float mag = sqrtf(dx * dx + dy * dy);
                if (mag == 0) {
                    return;
                }
                float spd = std::min(mag, mSpeed * m.data.s());
                pos.move(dx * spd / mag, dy * spd / mag);
            }
        });
}
