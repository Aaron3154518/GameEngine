#ifndef INCREMENTAL_H
#define INCREMENTAL_H

#include <Entities/Entity.h>
#include <Entities/EntityContainer.h>
#include <Framework/EventSystem/DragService.h>
#include <Framework/FrameworkComponents.h>
#include <Framework/RenderSystem/TextComponent.h>
#include <Messages/MessageBus.h>
#include <Observables/Observables.h>

enum CurrencyE : Messages::EnumT { Money = 0 };
ROOT_NODE(CurrencyVals, float, CurrencyE);

class Currency : public Entities::Entity {
   public:
   private:
    void init();
};

enum Tiers : Messages::EnumT { A = 0, B, C, D, E, F, G, H, I };
extern const std::vector<Tiers> TIERS;
ROOT_NODE(TierVals, float, Tiers);
ROOT_NODE(CostVals, float, Tiers);
ROOT_NODE(MultiVals, float, Tiers);

class UpButton : public Entities::Entity {
   public:
    UpButton(Tiers tier);

   private:
    void init();

    const Tiers mTier;
};

class Tier : public Entities::Entity {
   public:
    Tier(Tiers tier);

   private:
    void init();

    const Tiers mTier;
    std::unique_ptr<UpButton> mBtn;
};

class TierList : public Entities::EntityContainer<Tier> {
   public:
   private:
    void init();
};

#endif
