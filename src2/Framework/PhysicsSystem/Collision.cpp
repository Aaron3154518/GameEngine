#include "Collision.h"

// CollisionService
bool CollisionService::NewType(const Entities::UUID& idA,
                               const Entities::UUID& idB) {
    auto& cmap = GetCollisionMap();
    cmap[idA].push_back(idB);
    if (idA != idB) {
        cmap[idB].push_back(idA);
    }
    return true;
}

void CollisionService::manager_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const auto& m) { onUpdate(); }, EventSystem::Update);
}

// TODO: no double collision detection
void CollisionService::onUpdate() {
    auto& cmap = GetCollisionMap();
    auto& mb = Messages::GetMessageBus();
    auto iter = active<PositionComponent>();
    for (auto e1It = iter.begin(); e1It != iter.end(); ++e1It) {
        auto e1 = *e1It;
        auto& idA = e1.getData<CollisionService>();
        auto& ids = cmap[idA];
        auto e2It = e1It;
        for (++e2It; e2It != iter.end() && e1It; ++e2It) {
            auto e2 = *e2It;
            auto& idB = e2.getData<CollisionService>();
            if (std::find(ids.begin(), ids.end(), idB) == ids.end()) {
                continue;
            }
            SDL_Rect r;
            if (SDL_IntersectRect(e1.getData<PositionComponent>(),
                                  e2.getData<PositionComponent>(), &r)) {
                mb.sendMessage(Message(Collided, idB, {e1.id()}));
                mb.sendMessage(Message(Collided, idA, {e2.id()}));
            }
        }
    }
}

CollisionService::CollisionMap& CollisionService::GetCollisionMap() {
    static CollisionMap CMAP;
    return CMAP;
}
