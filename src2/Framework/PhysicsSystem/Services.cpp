#include "Services.h"

// PhysicsService
void PhysicsService::manager_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const auto& msg) { onUpdate(msg.data); }, EventSystem::Update);
}

void PhysicsService::onUpdate(Time dt) {
    float s = dt.s();
    float c = .5f * s * s;
    for (auto e : active<PositionComponent>()) {
        auto& pos = e.getData<PositionComponent>();
        if (auto v = e.getDataOpt<VelocityComponent>()) {
            pos.move(v->x * s, v->y * s);
            if (auto a = e.getDataOpt<AccelerationComponent>()) {
                pos.move(a->x * c, a->y * c);
                v->x += a->x * s;
                v->y += a->y * s;
            }
        }

        if (auto bounds = e.getDataOpt<BoundaryComponent>()) {
            pos.fitWithin(*bounds);
        }
    }
}