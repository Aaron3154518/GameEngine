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
typedef Observables::RootNode<float, CurrencyE> CurrencyVals;

class Currency : public Entities::Entity {
   public:
   private:
    void init();
};

enum Tiers : Messages::EnumT { A = 0, B, C, D, E, F, G, H, I };
extern const std::vector<Tiers> TIERS;
typedef Observables::RootNode<float, Tiers> TierVals;
typedef Observables::RootNode<int, Tiers> CostVals;
typedef Observables::RootNode<uint32_t, Tiers> MultiVals;

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
