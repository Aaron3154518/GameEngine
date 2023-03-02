#include "Wizards.h"

// Fireball
const Entities::UUID Fireball::CID = Entities::generateUUID();

void Fireball::update(Time dt) {
    auto& t = getComponent<TargetComponent>().get();
    auto& pos = getComponent<PositionComponent>().get();
    auto& p = getComponent<PhysicsService>();

    float dx = t.cX() - pos.cX(), dy = t.cY() - pos.cY();
    float r = fminf(sqrtf(dx * dx + dy * dy) / 2, p.maxV);

    if (fabsf(r) < 1e-5) {
        p.a = {0, 0};
        return;
    }

    float frac =
        fmaxf(p.v.x * p.v.x + p.v.y * p.v.y, p.maxV * p.maxV / 4) / r / r;
    p.a.x = dx * frac;
    p.a.y = dy * frac;
}

void Fireball::launch(SDL_FPoint from, float v,
                      const Observables::Node<Rect>& to) {
    auto& posComp = getComponent<PositionComponent>();
    Rect r = posComp.get();
    r.setPos(from.x, from.y, Rect::Align::CENTER);
    posComp.set(r);
    const Rect& t = to();
    float dx = t.cX() - from.x, dy = t.cY() - from.y;
    float theta = atan2f(dy, dx);

    static std::random_device rd;
    static std::mt19937_64 mt(rd());
    static std::uniform_real_distribution<float> dist(0, 1);

    theta += (dist(mt) - 0.5f) * M_PI * 3 / 2;

    auto& p = getComponent<PhysicsService>();
    p.v = {v * cosf(theta), v * sinf(theta)};
    p.maxV = v;
    getComponent<TargetComponent>().setSource(to);
}

void Fireball::init() {
    addComponent<PositionComponent>(Rect(0, 0, 25, 25));
    addComponent<TargetComponent>(Rect());
    addComponent<ElevationComponent>(1);
    addComponent<SpriteComponent>("res/projectiles/fireball.png");

    addComponent<RenderService>();
    addComponent<PhysicsService>(PhysicsData{});
    addComponent<CollisionService>(Fireball::CID);

    subscribeTo<CollisionService::Message>(
        [this](const CollisionService::Message& m) {
            Messages::GetMessageBus().sendMessage(
                FireballList::Message(FireballList::Remove, id()));
        },
        CollisionService::Collided);
}

// FireballList
void FireballList::container_init() {
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) {
            forEach(Fireball::update, m.data);
        },
        EventSystem::Update);
}

// Wizard
void Wizard::init() {
    WizPos pos(Wizards::Wizard);
    pos(Rect(25, -25, 50, 50));

    addComponent<PositionComponent>(pos);
    addComponent<ElevationComponent>(1);
    addComponent<SpriteComponent>("res/wizards/wizard_ss.png", 5, 150);
    addComponent<FireballListComponent>(GameObjects::New<FireballList>());

    addComponent<RenderService>();
    addComponent<PhysicsService>(PhysicsData{});

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

            auto& p = getComponent<PhysicsService>();
            switch (k.data.key) {
                case SDLK_a:
                    p.v.x -= maxV;
                    break;
                case SDLK_d:
                    p.v.x += maxV;
                    break;
                case SDLK_w:
                    p.v.y -= maxV;
                    break;
                case SDLK_s:
                    p.v.y += maxV;
                    break;
                default:
                    break;
            };
        });

    startTimer(2000, [this]() { shootFireball(); });
}

void Wizard::shootFireball() {
    auto& fList = getComponent<FireballListComponent>().get();
    fList.add();
    fList.back()->launch(
        getComponent<PositionComponent>().get().getPos(Rect::Align::CENTER),
        250, WizPos(Wizards::Crystal));
    startTimer(2000, [this]() { shootFireball(); });
}

// Crystal
const Entities::UUID Crystal::CID = Entities::generateUUID();

void Crystal::init() {
    WizPos pos(Wizards::Crystal);
    pos(Rect(-25, -25, 50, 50));

    addComponent<PositionComponent>(pos);
    addComponent<ElevationComponent>(0);
    addComponent<SpriteComponent>("res/wizards/crystal.png");

    addComponent<RenderService>();
    addComponent<CollisionService>(Crystal::CID);

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
