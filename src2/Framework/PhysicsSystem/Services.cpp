#include "Services.h"

// PhysicsService
void PhysicsService::manager_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const auto& msg) { onUpdate(msg.data); }, EventSystem::Update);
}

void PhysicsService::onUpdate(Time dt) {
    float s = dt.s();
    float c = .5f * s * s;
    for (auto data : *this) {
        auto& pos = data.pos->get();
        auto& v = data.v->get();
        auto& a = data.a->get();
        pos.move(v.x * s + a.x * c, v.y * s + a.y * c);
        v.x += a.x * s;
        v.y += a.y * s;

        auto& bounds = data.bounds->get();
        if (!bounds.empty()) {
            pos.fitWithin(bounds);
        }
    }
}
