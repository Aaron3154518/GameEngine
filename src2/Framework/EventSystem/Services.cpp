#include "Services.h"

// UpdateService
REGISTER(UpdateService::Message, UpdateMessage, {
    std::stringstream ss(line);
    int cnt;
    return std::make_unique<Msg>(code,
                                 ss >> cnt ? std::max(0, cnt) : 1.0 / 60.0);
});

// PhysicsService
void PhysicsService::service_init() {
    subscribeTo<UpdateService::Message>(
        [this](const auto& msg) { onUpdate(msg.data); }, UpdateService::Update);
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
