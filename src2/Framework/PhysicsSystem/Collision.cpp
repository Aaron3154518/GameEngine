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
    auto iter = active<PositionComponent, CollisionComponent>();
    for (auto e1 : iter) {
        auto& idA = e1.getData<CollisionComponent>();
        auto& ids = cmap[idA];
        for (auto e2 : iter) {
            auto& idB = e2.getData<CollisionComponent>();
            if (e2.id() == e1.id() ||
                std::find(ids.begin(), ids.end(), idB) == ids.end()) {
                continue;
            }
            SDL_Rect r;
            if (SDL_IntersectRect(e1.getData<PositionComponent>(),
                                  e2.getData<PositionComponent>(), &r)) {
                mb.sendMessage(Message(Collided, idB, {e1.id()}));
                mb.sendMessage(Message(Collided, idA, {e2.id()}));
                break;
            }
        }
    }
}

CollisionService::CollisionMap& CollisionService::GetCollisionMap() {
    static CollisionMap CMAP;
    return CMAP;
}
