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
        auto pos = e.getData<PositionComponent>();
        auto& data = e.get<PhysicsService>();
        pos.move(data.v.x * s + data.a.x * c, data.v.y * s + data.a.y * c);
        data.v.x += data.a.x * s;
        data.v.y += data.a.y * s;
        if (data.maxV >= 0) {
            float vMag = sqrtf(data.v.x * data.v.x + data.v.y * data.v.y);
            if (vMag > 0) {
                data.v.x *= data.maxV / vMag;
                data.v.y *= data.maxV / vMag;
            }
        }

        if (auto bounds = e.getDataOpt<BoundaryComponent>()) {
            pos.fitWithin(*bounds);
        }
        e.setData<PositionComponent>(pos);
    }
}