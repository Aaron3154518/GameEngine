#include "Collision.h"

// CollisionComponentMqnager
std::vector<Entities::UUID> CollisionComponent::getEntities(
    const Entities::UUID& colId) {
    std::vector<Entities::UUID> vec;
    for (auto it = begin(), e = end(); it != e; ++it) {
        if (it->isActive() && colId == it->get()) {
            vec.push_back(it.id());
        }
    }
    return vec;
}

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

// TODO: optimize accesses, no double collision detection
void CollisionService::onUpdate() {
    auto& cmap = GetCollisionMap();
    auto& mb = Messages::GetMessageBus();
    auto& posMan = GameObjects::Get<PositionComponent>();
    auto& colMan = GameObjects::Get<CollisionComponent>();
    for (auto it = begin(); it != end(); ++it) {
        if (!it->isActive()) {
            continue;
        }
        auto& id = it.id();
        Rect pos = posMan[id].get();
        auto& idA = colMan[id].get();
        for (auto idB : cmap[idA]) {
            for (auto eId : colMan.getEntities(idB)) {
                SDL_Rect r;
                if (SDL_IntersectRect(pos, posMan[eId].get(), &r)) {
                    mb.sendMessage(Message(Collided, idB, {id}));
                    mb.sendMessage(Message(Collided, idA, {eId}));
                }
            }
        }
    }
}

CollisionService::CollisionMap& CollisionService::GetCollisionMap() {
    static CollisionMap CMAP;
    return CMAP;
}
