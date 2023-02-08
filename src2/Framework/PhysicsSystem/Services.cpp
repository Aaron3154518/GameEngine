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

void PhysicsService2::manager_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const auto& msg) {
            float s = msg.data.s();
            float c = .5f * s * s;
            forEach([s, c](PositionComponent::Component& pC,
                           VelocityComponent::Component& vC,
                           AccelerationComponent::Component& aC,
                           BoundaryComponent::Component& bC, int i) {
                auto& pos = pC.get();
                auto& v = vC.get();
                auto& a = aC.get();
                auto& bounds = bC.get();
                pos.move(v.x * s + a.x * c, v.y * s + a.y * c);
                v.x += a.x * s;
                v.y += a.y * s;

                if (!bounds.empty()) {
                    pos.fitWithin(bounds);
                }
            });
        },
        EventSystem::Update);
}
