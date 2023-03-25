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
class CollisionService
    : public Services::Service<Components::DataComponent<Entities::UUID>> {
   public:
    typedef std::unordered_map<Entities::UUID, std::vector<Entities::UUID>>
        CollisionMap;

    DATA_MESSAGE(Message, Entities::UUID, Collided);

    static bool NewType(const Entities::UUID& idA, const Entities::UUID& idB);

    static void triggerImmunity(Entities::Entity* e, int len_ms);

   private:
    void manager_init();

    void onUpdate();

    static CollisionMap& GetCollisionMap();
};

#endif