#ifndef INCREMENTAL_H
#define INCREMENTAL_H

#include <Entities/EntitySystem.h>
#include <Framework/EventSystem/DragService.h>
#include <Framework/FrameworkComponents.h>
#include <Framework/RenderSystem/TextComponent.h>
#include <Messages/MessageBus.h>
#include <Observables/Observables.h>
#include <Utils/Number.h>

enum CurrencyE : Observables::EnumT { Money = 0 };
ROOT_NODE(CurrencyVals, Number, CurrencyE);

class Currency : public Entities::Entity {
   public:
   private:
    void init();
};

enum Tiers : Observables::EnumT { A = 0, B, C, D, E, F, G, H, I };
extern const std::vector<Tiers> TIERS;
ROOT_NODE(TierVals, Number, Tiers);
ROOT_NODE(CostVals, Number, Tiers);
ROOT_NODE(MultiVals, Number, Tiers);

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
