#ifndef COLLISION_H
#define COLLISION_H

#include <Components/ComponentManager.h>
#include <Entities/UUID.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/FrameworkComponents.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>

#include <unordered_set>

class CollisionComponent : public Components::Component {
   public:
    CollisionComponent(const Entities::UUID& id);

    Entities::UUID mId;
};

class CollisionComponentManager
    : public Components::ComponentManager<CollisionComponent> {
   public:
    std::vector<Entities::UUID> getEntities(const Entities::UUID& colId);
};

class CollisionService : public Services::Service {
   public:
    struct CollisionType {
        Entities::UUID idA, idB, cId;
    };

    typedef std::unordered_multiset<CollisionType> CollisionMap;

    DATA_MESSAGE(Message, CollisionType, Collided);

    static Entities::UUID NewType(const Entities::UUID& idA,
                                  const Entities::UUID& idB);

   private:
    void service_init();

    void onUpdate();

    static CollisionMap& GetCollisionMap();
};

bool operator==(const CollisionService::CollisionType& lhs,
                const CollisionService::CollisionType& rhs);

namespace std {
template <>
struct hash<CollisionService::CollisionType> {
    size_t operator()(const CollisionService::CollisionType& c) const {
        return hash<Entities::UUID>()(c.idA);
    }
};
}  // namespace std

#endif