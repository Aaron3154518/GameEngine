#include "Services.h"

// PhysicsService
void PhysicsService::service_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const auto& msg) { onUpdate(msg.data); }, EventSystem::Update);
}

void PhysicsService::onUpdate(Time dt) {
    auto& posMan = GameObjects::Get<PositionComponentManager>();
    auto& velMan = GameObjects::Get<VelComponentManager>();
    auto& accelMan = GameObjects::Get<AccelComponentManager>();
    float s = dt.s();
    float c = .5f * s * s;
    for (auto& e : mEntities) {
        Rect& pos = posMan[e].get();
        if (velMan.hasEntity(e)) {
            SDL_FPoint& v = velMan[e].get();
            pos.move(v.x * s, v.y * s);
            if (accelMan.hasEntity(e)) {
                SDL_FPoint& a = accelMan[e].get();
                pos.move(c * a.x, c * a.y);
                v.x += a.x * s;
                v.y += a.y * s;
            }
        }
    }
}
