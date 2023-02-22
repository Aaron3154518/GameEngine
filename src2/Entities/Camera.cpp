#include "Camera.h"

#include <Framework/RenderSystem/Services.h>

// Camera::Tracker
Rect Camera::Tracker::operator()(Rect pos, const Rect& target, Time dt) {
    return pos;
}

// Camera::ConstantTracker
Camera::ConstantTracker::ConstantTracker(float spd) : mMaxSpeed(spd) {}

Rect Camera::ConstantTracker::operator()(Rect pos, const Rect& target,
                                         Time dt) {
    float dx = target.cX() - pos.cX(), dy = target.cY() - pos.cY();
    float mag = sqrtf(dx * dx + dy * dy);
    if (mag == 0) {
        return pos;
    }
    float spd = std::min(mag, mMaxSpeed * dt.s());
    pos.move(dx * spd / mag, dy * spd / mag);
    return pos;
}

// Camera::ScaleTracker
Camera::ScaleTracker::ScaleTracker(float a) : mA(a) {}

Rect Camera::ScaleTracker::operator()(Rect pos, const Rect& target, Time dt) {
    float dx = target.cX() - pos.cX(), dy = target.cY() - pos.cY();
    float mag = sqrtf(dx * dx + dy * dy);
    if (mag == 0) {
        return pos;
    }
    float spd = std::max(std::min(1.0f, mag), mag * mA * dt.s());
    pos.move(dx * spd / mag, dy * spd / mag);
    return pos;
}

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

void Camera::track(const Entities::UUID& eId, TrackerPtr tracker) {
    mTrackId = eId;
    mTracker = std::move(tracker);
}

const Entities::UUID& Camera::getTracking() const { return mTrackId; }

void Camera::init() {
    Dimensions d = RenderSystem::getWindowSize();
    addComponent<PositionComponent>(Rect(0, 0, d.w, d.h));

    subscribeTo<RenderService::Message>(
        [this](const RenderService::Message& m) {
            if (mTrackId != Entities::NullId() && mTracker) {
                Rect& tPos =
                    GameObjects::Get<PositionComponent>()[mTrackId].get();
                Rect& pos = getComponent<PositionComponent>().get();
                pos = (*mTracker)(pos, tPos, EventSystem::get().dt);
            }
        },
        RenderService::PreRender);
}
