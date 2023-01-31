#include "Collision.h"

// CollisionComponent
CollisionComponent::CollisionComponent(const Entities::UUID& id) : mId(id) {}

// CollisionComponentMqnager
std::vector<Entities::UUID> CollisionComponentManager::getEntities(
    const Entities::UUID& colId) {
    std::vector<Entities::UUID> vec;
    for (auto it = begin(), e = end(); it != e; ++it) {
        if (colId == it->mId) {
            vec.push_back(it.id());
        }
    }
    return vec;
}

// CollisionService::CollisionType
bool operator==(const CollisionService::CollisionType& lhs,
                const CollisionService::CollisionType& rhs) {
    return lhs.idA == rhs.idA && lhs.idB == rhs.idB && lhs.cId == rhs.cId;
}

// CollisionService
Entities::UUID CollisionService::NewType(const Entities::UUID& idA,
                                         const Entities::UUID& idB) {
    auto cId = Entities::generateUUID();
    auto& cmap = GetCollisionMap();
    cmap.insert({idA, idB, cId});
    if (idA != idB) {
        cmap.insert({idB, idA, cId});
    }
    return cId;
}

void CollisionService::service_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const auto& m) { onUpdate(); }, EventSystem::Update);
}

// TODO: optimize accesses, no double collision detection
void CollisionService::onUpdate() {
    auto& cmap = GetCollisionMap();
    auto& mb = Messages::GetMessageBus();
    auto& posMan = GameObjects::Get<PositionComponentManager>();
    auto& colMan = GameObjects::Get<CollisionComponentManager>();
    for (auto& id : mEntities) {
        Rect pos = posMan[id].get();
        auto colId = colMan[id].mId;
        auto b = cmap.bucket({colId});
        for (auto it = cmap.begin(b), end = cmap.end(b); it != end; ++it) {
            for (auto eId : colMan.getEntities(it->idB)) {
                SDL_Rect r;
                if (SDL_IntersectRect(pos, posMan[eId].get(), &r)) {
                    mb.sendMessage(Message(Collided, *it, {id}));
                    mb.sendMessage(Message(Collided, *it, {eId}));
                }
            }
        }
    }
}

CollisionService::CollisionMap& CollisionService::GetCollisionMap() {
    static CollisionMap CMAP;
    return CMAP;
}
