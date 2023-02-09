#include "Services.h"

// PhysicsService
void PhysicsService::manager_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const auto& msg) { onUpdate(msg.data); }, EventSystem::Update);
}

void PhysicsService::onUpdate(Time dt) {
    float s = dt.s();
    float c = .5f * s * s;
    for (auto e : require<PositionComponent>()) {
        auto& pos = e.getData<PositionComponent>();
        auto& v = e.getData<VelocityComponent>();
        auto& a = e.getData<AccelerationComponent>();
        pos.move(v.x * s + a.x * c, v.y * s + a.y * c);
        v.x += a.x * s;
        v.y += a.y * s;

        auto& bounds = e.getData<BoundaryComponent>();
        if (!bounds.empty()) {
            pos.fitWithin(bounds);
        }
    }
}