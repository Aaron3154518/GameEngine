#ifndef COLLISION_H
#define COLLISION_H

#include <Components/ComponentManager.h>
#include <Entities/UUID.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/FrameworkComponents.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>

#include <unordered_set>

// Stores a collision id
class CollisionComponent : public Components::ComponentManager<
                               Components::DataComponent<Entities::UUID>> {
   public:
    std::vector<Entities::UUID> getEntities(const Entities::UUID& colId);
};

class CollisionService : public Services::Service {
   public:
    typedef std::unordered_map<Entities::UUID, std::vector<Entities::UUID>>
        CollisionMap;

    DATA_MESSAGE(Message, Entities::UUID, Collided);

    static bool NewType(const Entities::UUID& idA, const Entities::UUID& idB);

   private:
    void manager_init();

    void onUpdate();

    static CollisionMap& GetCollisionMap();
};

#endif