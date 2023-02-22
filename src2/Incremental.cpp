#include "Incremental.h"

#include <iostream>
#include <string>
#include <vector>

// Currency
void Currency::init() {
    CurrencyVals c(CurrencyE::Money);
    c(1);

    Rect r(200, 0, 100, 50);
    static SharedFont font =
        AssetManager::getFont({r.W(), r.H(), "99999999999"});

    addComponent<TextComponent>();
    auto& text = getComponent<TextComponent>();
    text.setElevation(1);
    text.setText("{i}");
    text.setImages({SpriteData("res/wizards/crystal.png")});
    text.setRect(r);
    text.setFont(font);

    Observables::subscribe(
        this,
        [this](float val) {
            auto& text = getComponent<TextComponent>();
            text.setText("{i}" + std::to_string(val));
        },
        c);
}

const std::vector<Tiers> TIERS{Tiers::A, Tiers::B, Tiers::C, Tiers::D, Tiers::E,
                               Tiers::F, Tiers::G, Tiers::H, Tiers::I};

// Tier
Tier::Tier(Tiers tier) : mTier(tier) {}

void Tier::init() {
    Rect r(200, (mTier + 1) * 50, 100, 50);
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

    Observables::subscribe(
        this,
        [r, this](float val) {
            auto& text = getComponent<TextComponent>();
            text.setText("{i}" + std::to_string(val));
        },
        TierVals(mTier));

    mBtn = GameObjects::New<UpButton>(mTier);
}

// TierList
void TierList::init() {
    for (Tiers t : TIERS) {
        add(t);
    }

    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) {
            CurrencyVals c(CurrencyE::Money);
            c(c() +
              TierVals(TIERS.at(0))() * m.data.s() * MultiVals(TIERS.at(0))());

            for (size_t i = 0; i < TIERS.size() - 1; i++) {
                TierVals v1(TIERS.at(i)), v2(TIERS.at((i + 1) % TIERS.size()));
                v1(v1() + v2() * m.data.s() * MultiVals(TIERS.at(i + 1))());
            }
        },
        EventSystem::Update);
}

// UpButton
UpButton::UpButton(Tiers tier) : mTier(tier) {}

void UpButton::init() {
    TierVals t(mTier);

    CostVals c(mTier);
    c(std::pow(13, (int)mTier));

    MultiVals m(mTier);
    m(1);

    Rect r(300, (mTier + 1) * 50, 50, 50);
    static SharedFont font =
        AssetManager::getFont({r.W(), r.H(), "99999999999"});

    addComponent<PositionComponent>(r);
    addComponent<ElevationComponent>(2);

    addComponent<TextComponent>();
    auto& text = getComponent<TextComponent>();
    text.setElevation(2);
    text.setFont(font);
    text.setRect(r);

    TextureBuilder tex;
    Shapes::Rectangle texRect;
    texRect.set(r, -3);
    tex.draw(texRect);
    addComponent<SpriteComponent>(tex.getTexture());
    addComponent<RenderService>();

    Observables::subscribe(
        this,
        [this](float cost) {
            auto& text = getComponent<TextComponent>();
            text.setText(std::to_string(cost));
        },
        c);

    addComponent<MouseService>();
    subscribeTo<MouseService::MouseMessage>(
        [this, t, m, c](const MouseService::MouseMessage& msg) {
            if (msg.data.mouse == Event::LEFT && msg.opts.target == id()) {
                CurrencyVals curr(CurrencyE::Money);
                if (curr() < c()) {
                    return;
                }
                curr(curr() - c());

                if (t() == 0) {
                    t(1);
                } else {
                    m(m() * 2);
                }
                c(c() * 10);
            }
        },
        Event::CLICKED);
}
