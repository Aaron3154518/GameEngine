#include "Wizards.h"

// Wizard
void Wizard::init() {
    addComponent<PositionComponent>(Rect());
    addComponent<VelocityComponent>(SDL_FPoint{});
    addComponent<ElevationComponent>(1);
    addComponent<SpriteComponent>("res/wizards/wizard_ss.png", 5, 150);

    addComponent<RenderService>();
    addComponent<PhysicsService>();

    WizPos pos(Wizards::Wizard);
    Observables::subscribe(
        this,
        [this](const Rect& pos) { getComponent<PositionComponent>().set(pos); },
        pos);
    pos(Rect(25, -25, 50, 50));
    subscribeTo<EventSystem::UpdateMessage>(
        [this, pos](const auto& m) {
            pos(getComponent<PositionComponent>().get());
        },
        EventSystem::Update);

    const static float V = 100;
    subscribeTo<EventSystem::KeyboardMessage>(
        [this](const EventSystem::KeyboardMessage& k) {
            float maxV = 0;
            if (k.code == Event::DOWN) {
                maxV = V;
            } else if (k.code == Event::UP) {
                maxV = -V;
            } else {
                return;
            }

            auto& v = getComponent<VelocityComponent>().get();
            switch (k.data.key) {
                case SDLK_a:
                    v.x -= maxV;
                    break;
                case SDLK_d:
                    v.x += maxV;
                    break;
                case SDLK_w:
                    v.y -= maxV;
                    break;
                case SDLK_s:
                    v.y += maxV;
                    break;
                default:
                    break;
            };
        });
}

// Crystal
void Crystal::init() {
    addComponent<PositionComponent>(Rect());
    addComponent<ElevationComponent>(0);
    addComponent<SpriteComponent>("res/wizards/crystal.png");
    addComponent<RenderService>();

    WizPos pos(Wizards::Crystal);
    Observables::subscribe(
        this,
        [this](const Rect& pos) { getComponent<PositionComponent>().set(pos); },
        pos);
    pos(Rect(-25, -25, 50, 50));

    WizPos wizPos(Wizards::Wizard);
    Observables::subscribe(
        this,
        [pos, wizPos]() {
            auto& camera = Camera::Get();
            auto& id = camera.getTracking();

            auto& wPos = wizPos();
            auto& cPos = pos();
            float dx = wPos.cX() - cPos.cX(), dy = wPos.cY() - cPos.cY();
            float mag = sqrtf(dx * dx + dy * dy);

            auto wiz = GameObjects::Get<Wizard>().id();
            auto crys = GameObjects::Get<Crystal>().id();
            if ((id != crys && id != wiz) || (id == wiz && mag <= 245)) {
                camera.track(crys, 250);
            } else if (id == crys && mag > 255) {
                camera.track(wiz, 200);
            }
        },
        pos, wizPos);
}

// Boundary
void Boundary::init() {
    Rect r(0, 0, 500, 500);
    addComponent<PositionComponent>(r);
    addComponent<ElevationComponent>(0);
    TextureBuilder tex(r.W(), r.H());
    Shapes::Circle c;
    c.setCenter(r.GetPos(Rect::Align::CENTER));
    c.setRadius((int)r.halfW(), -3);
    c.setDashed(25);
    tex.draw(c);
    addComponent<SpriteComponent>(tex.getTexture());
    addComponent<RenderService>();

    WizPos pos(Wizards::Crystal);
    Observables::subscribe(
        this,
        [this](const Rect& pos) {
            getComponent<PositionComponent>().get().setPos(pos,
                                                           Rect::Align::CENTER);
        },
        pos);
}
