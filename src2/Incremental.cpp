#include "Incremental.h"

#include <iostream>
#include <string>
#include <vector>

const std::vector<Tiers> TIERS{Tiers::A, Tiers::B, Tiers::C, Tiers::D,
                               Tiers::E, Tiers::F, Tiers::G};

// Tier
Tier::Tier(Tiers tier) : mTier(tier) {}

void Tier::init() {
    Rect r(200, mTier * 50, 100, 50);
    static SharedFont font =
        AssetManager::getFont({r.W(), r.H(), "99999999999"});

    addComponent<ElevationComponent>(1);
    addComponent<PositionComponent>(r);
    addComponent<TextComponent>();
    auto& text = getComponent<TextComponent>();
    text.setElevation(1);
    text.setText("{i}");
    text.setImages({mTier % 2 == 0
                        ? SpriteData("res/projectiles/fireball.png")
                        : SpriteData("res/wizards/wizard_ss.png", 5, 150)});
    text.setRect(r);
    text.setFont(font);

    addComponent<MouseService>();
    subscribeTo<MouseService::MouseMessage>(
        [this](const MouseService::MouseMessage& m) {
            if (m.data.mouse == Event::LEFT && m.opts.target == id()) {
                TierVals t(mTier);
                t(t() + 1);
            }
        },
        Event::CLICKED);

    Observables::subscribe(
        this,
        [r, this](float val) {
            auto& text = getComponent<TextComponent>();
            text.setText("{i}" + std::to_string(val));
        },
        TierVals(mTier));
}

// TierList
void TierList::init() {
    for (Tiers t : TIERS) {
        add(t);
    }

    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) {
            for (size_t i = 0; i < TIERS.size() - 1; i++) {
                TierVals v1(TIERS.at(i)), v2(TIERS.at((i + 1) % TIERS.size()));
                v1(v1() + v2() * m.data.s());
            }
        },
        EventSystem::Update);
}
