#ifndef INCREMENTAL_H
#define INCREMENTAL_H

#include <Entities/Entity.h>
#include <Entities/EntityContainer.h>
#include <Framework/EventSystem/DragService.h>
#include <Framework/FrameworkComponents.h>
#include <Framework/RenderSystem/TextComponent.h>
#include <Messages/MessageBus.h>
#include <Observables/Observables.h>

enum Tiers : Messages::EnumT { A = 0, B, C, D, E, F, G };
extern const std::vector<Tiers> TIERS;
typedef Observables::RootNode<float, Tiers> TierVals;

class Tier : public Entities::Entity {
   public:
    Tier(Tiers tier);

   private:
    void init();

    const Tiers mTier;
};

class TierList : public Entities::EntityContainer<Tier> {
   public:
   private:
    void init();
};

#endif
