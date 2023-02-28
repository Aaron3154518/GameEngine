#include "Wizards.h"

// Fireball
const Entities::UUID Fireball::CID = Entities::generateUUID();

void Fireball::update(Time dt) {
    auto& t = WizPos(Wizards::Crystal)();
    auto& pos = getComponent<PositionComponent>().get();
    float dx = t.cX() - pos.cX(), dy = t.cY() - pos.cY();
    float mag = sqrtf(dx * dx + dy * dy);
    auto& aComp = getComponent<AccelerationComponent>();
    auto a = aComp.get();
    if (mag == 0) {
        a.x = 0;
        a.y = 0;
    } else {
        a.x = dx * 100 / mag;
        a.y = dy * 100 / mag;
    }
    aComp.set(a);
}

void Fireball::init() {
    addComponent<PositionComponent>(Rect(0, 0, 50, 50));
    addComponent<VelocityComponent>(SDL_FPoint{});
    addComponent<AccelerationComponent>(SDL_FPoint{});
    addComponent<ElevationComponent>(1);
    addComponent<SpriteComponent>("res/projectiles/fireball.png");
    addComponent<CollisionComponent>(CID);

    addComponent<RenderService>();
    addComponent<PhysicsService>();
    addComponent<CollisionService>();

    subscribeTo<CollisionService::Message>(
        [this](const CollisionService::Message& m) {
            Messages::GetMessageBus().sendMessage(
                FireballList::Message(FireballList::Remove, id()));
        },
        CollisionService::Collided);
}

// FireballList
void FireballList::init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) {
            forEach(Fireball::update, m.data);
        },
        EventSystem::Update);
}

// Wizard
void Wizard::init() {
    addComponent<PositionComponent>(Rect());
    addComponent<VelocityComponent>(SDL_FPoint{});
    addComponent<ElevationComponent>(1);
    addComponent<SpriteComponent>("res/wizards/wizard_ss.png", 5, 150);
    addComponent<FireballListComponent>(GameObjects::New<FireballList>());

    addComponent<RenderService>();
    addComponent<PhysicsService>();

    WizPos pos(Wizards::Wizard);
    pos(Rect(25, -25, 50, 50));
    getComponent<PositionComponent>().setSource(pos);

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

            auto v = getComponent<VelocityComponent>().get();
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
            getComponent<VelocityComponent>().set(v);
        });

    startTimer(2000, [this]() { shootFireball(); });
}

void Wizard::shootFireball() {
    getComponent<FireballListComponent>().get().add();
    startTimer(2000, [this]() { shootFireball(); });
}

// Crystal
const Entities::UUID Crystal::CID = Entities::generateUUID();

void Crystal::init() {
    addComponent<PositionComponent>(Rect());
    addComponent<ElevationComponent>(0);
    addComponent<SpriteComponent>("res/wizards/crystal.png");
    addComponent<CollisionComponent>(Crystal::CID);

    addComponent<RenderService>();
    addComponent<CollisionService>();

    WizPos pos(Wizards::Crystal);
    pos(Rect(-25, -25, 50, 50));
    getComponent<PositionComponent>().setSource(pos);

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
                camera.track(crys,
                             std::make_unique<Camera::ConstantTracker>(250));
            } else if (id == crys && mag > 255) {
                camera.track(wiz, std::make_unique<Camera::ScaleTracker>(2));
            }
        },
        pos, wizPos);

    subscribeTo<CollisionService::Message>(
        [this](const CollisionService::Message& m) {},
        CollisionService::Collided);
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
            auto& posComp = getComponent<PositionComponent>();
            auto rect = posComp.get();
            rect.setPos(pos, Rect::Align::CENTER);
            posComp.set(rect);
        },
        pos);
}

namespace {
const bool _ = CollisionService::NewType(Crystal::CID, Fireball::CID);
}
