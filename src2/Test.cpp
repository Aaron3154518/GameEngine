#include "Test.h"

Entities::UUID P = Entities::generateUUID();
Entities::UUID E = Entities::generateUUID();
Rect BOUND(0, 0, 500, 500);
bool _ = CollisionService::NewType(P, E);

int MyService::count() const { return mCnt; }

// MyService
void MyService::manager_init() {
    subscribeTo<CountMessage>([this](const CountMessage& m) { mCnt += m.data; },
                              IncreaseCount);
}

REGISTER(MyService::Message, MyMessage);

REGISTER(MyService::CountMessage, MyCountMessage, {
    std::stringstream ss(line);
    int cnt;
    return std::make_unique<Msg>(code,
                                 code == MyService::PrintCount
                                     ? GameObjects::Get<MyService>().count()
                                 : !(ss >> cnt) ? 1
                                                : cnt);
});

// MyEntity
void MyEntity::init() {
    MyRoots a(RootVals::A);
    MyStems b(StemVals::B);
    Observables::subscribe<>(
        this,
        [a, b]() {
            std::cerr << "A = " << a() << ", B = " << b() << std::endl;
        },
        b);
    b.subscribeTo([a]() { return "Hello: " + std::to_string(a()); }, a);
    a.set(5);

    addComponent<Components::ComponentManager<Components::Component>>();
    addComponent<MyComponentManager>();
    subscribeTo<MyService::Message>(
        [](const MyService::Message& m) {
            Messages::GetMessageBus().sendMessage(
                MyService::Message(MyService::World));
        },
        MyService::Hello);

    addComponent<PositionComponent>(Rect(10, 10, 50, 50));
    addComponent<VelocityComponent>(SDL_FPoint{0, 0});
    addComponent<AccelerationComponent>(SDL_FPoint{0, 0});
    addComponent<BoundaryComponent>(BOUND);
    addComponent<PhysicsService>();

    Camera::Get().track(id(), 75);

    addComponent<ElevationComponent>(1);
    addComponent<SpriteComponent>("res/wizards/wizard_ss.png", 5, 150);
    addComponent<RenderService>();
    Rect tr(190, 10, 120, 50);
    addComponent<TextComponent>(
        "Hello World\n{i}", std::vector<std::string>{"res/wizards/wizard.png"},
        tr, AssetManager::getFont({-1, 25, "|"}), Rect::Align::CENTER,
        Rect::Align::TOP_LEFT);
    TextureBuilder tex(tr.W(), tr.H());
    Shapes::Rectangle r;
    r.set(Rect(0, 0, tr.W(), tr.H()), -3, false);
    tex.draw(r);
    addComponent<SpriteComponent>(tex.getTexture());
    addComponent<PositionComponent>(tr);

    addComponent<HealthComponent>(5, Rect(450, 450, 50, 50));
    addComponent<CollisionComponent>(P);
    addComponent<CollisionService>();
    subscribeTo<CollisionService::Message>(
        [this](const CollisionService::Message& m) {
            auto& hp = getComponent<HealthComponent>();
            --hp;
            if (hp.get() <= 0) {
                hp = 5;
                getComponent<PositionComponent>().get() = Rect(10, 10, 50, 50);
            }
            getComponent<CollisionComponent>().setActive(false);
            startTimer(2000, [this]() {
                getComponent<CollisionComponent>().setActive(true);
            });
        },
        CollisionService::Collided);

    subscribeTo<EventSystem::KeyboardMessage>(
        [this](const EventSystem::KeyboardMessage& m) {
            auto& pos = getComponent<PositionComponent>().get();
            auto& camera = Camera::Get();
            switch (m.data.key) {
                case SDLK_r:
                    pos = Rect(10, 10, 50, 50);
                    break;
                case SDLK_MINUS:
                    camera.getRect().resize(1 / 0.9, Rect::Align::CENTER);
                    break;
                case SDLK_EQUALS:
                    camera.getRect().resize(0.9, Rect::Align::CENTER);
                    break;
                default:
                    break;
            }
        },
        Event::PRESSED);
    static int V = 100;
    subscribeTo<EventSystem::KeyboardMessage>(
        [this](const EventSystem::KeyboardMessage& m) {
            auto& v = getComponent<VelocityComponent>().get();
            switch (m.data.key) {
                case SDLK_a:
                    v.x -= V;
                    break;
                case SDLK_d:
                    v.x += V;
                    break;
                case SDLK_w:
                    v.y -= V;
                    break;
                case SDLK_s:
                    v.y += V;
                    break;
                default:
                    break;
            }
        },
        Event::DOWN);
    subscribeTo<EventSystem::KeyboardMessage>(
        [this](const EventSystem::KeyboardMessage& m) {
            auto& v = getComponent<VelocityComponent>().get();
            switch (m.data.key) {
                case SDLK_a:
                    v.x += V;
                    break;
                case SDLK_d:
                    v.x -= V;
                    break;
                case SDLK_w:
                    v.y += V;
                    break;
                case SDLK_s:
                    v.y -= V;
                    break;
                default:
                    break;
            }
        },
        Event::UP);

    addComponent<MouseService>(MouseOptions(true));
    subscribeTo<MouseService::MouseMessage>(
        [this](const MouseService::MouseMessage& m) {
            if (m.data.mouse == Event::LEFT &&
                m.opts.target == Entities::NullId()) {
                getComponent<PositionComponent>().get().setPos(
                    m.data.clickPos.x, m.data.clickPos.y, Rect::Align::CENTER);
            }
        },
        Event::CLICKED);
    subscribeTo<MouseService::DragMessage>(
        [this](const MouseService::DragMessage& m) {
            auto& r = getComponent<PositionComponent>().get();
            r.setPos(m.data.x, m.data.y, Rect::Align::CENTER);
        },
        MouseService::Dragging);
}

// EntityProj
void EnemyProj::init() {
    addComponent<PositionComponent>(Rect(0, 0, 20, 20));
    addComponent<VelocityComponent>(SDL_FPoint{0, 0});
    addComponent<PhysicsService>();

    addComponent<ElevationComponent>(2);
    addComponent<SpriteComponent>("res/wizards/catalyst.png");
    addComponent<RenderService>();

    addComponent<CollisionComponent>(E);
    addComponent<CollisionService>();
    subscribeTo<CollisionService::Message>(
        [this](const auto& m) {
            Messages::GetMessageBus().sendMessage(
                EnemyProjCont::Message(EnemyProjCont::Remove, id()));
        },
        CollisionService::Collided);
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const auto& m) {
            auto& pos = getComponent<PositionComponent>().get();
            SDL_Rect _;
            if (!SDL_IntersectRect(pos, BOUND, &_)) {
                Messages::GetMessageBus().sendMessage(EnemyProjCont::Message(
                    EnemyProjCont::Remove, id(), {Entities::NullId(), true}));
            }
        },
        EventSystem::Update);
}

// Enemy
void Enemy::init() {
    mProjs = GameObjects::New<EnemyProjCont>();

    std::mt19937 gen = std::mt19937(rand());
    std::uniform_real_distribution<float> rDist;

    addComponent<PositionComponent>(
        Rect(rDist(gen) * 450, rDist(gen) * 450, 50, 50));
    addComponent<VelocityComponent>(SDL_FPoint{0, 0});
    addComponent<BoundaryComponent>(BOUND);
    addComponent<PhysicsService>();

    addComponent<ElevationComponent>(0);
    addComponent<SpriteComponent>("res/wizards/crystal.png");
    addComponent<RenderService>();

    addComponent<CollisionComponent>(E);
    addComponent<CollisionService>();

    const float V = rDist(gen) * 50;
    subscribeTo<EventSystem::UpdateMessage>(
        [this, V](const auto& m) {
            auto& v = getComponent<VelocityComponent>().get();
            auto& pos = getComponent<PositionComponent>().get();
            auto& player = GameObjects::Get<PositionComponent>()
                               [GameObjects::Get<MyEntity, MyEntity::Player>()]
                                   .get();
            v.x = player.cX() - pos.cX();
            v.y = player.cY() - pos.cY();
            float mag = sqrtf(v.x * v.x + v.y * v.y);
            if (mag > 0) {
                v.x *= V / mag;
                v.y *= V / mag;
            }
        },
        EventSystem::Update);

    subscribeTo<EventSystem::UpdateMessage>(
        [this](const EventSystem::UpdateMessage& m) {
            timer -= m.data.ms();
            if (timer <= 0) {
                timer += 1500;
                mProjs->add();

                auto c = getComponent<PositionComponent>().get().getPos(
                    Rect::Align::CENTER);
                auto t = GameObjects::Get<PositionComponent>()
                             [GameObjects::Get<MyEntity, MyEntity::Player>()]
                                 .get()
                                 .getPos(Rect::Align::CENTER);
                SDL_FPoint v{t.x - c.x, t.y - c.y};
                float mag = sqrtf(v.x * v.x + v.y * v.y) / 150;
                if (mag == 0) {
                    mag = 1;
                }
                GameObjects::Get<PositionComponent>()[mProjs->back()->id()]
                    .get()
                    .setPos(c.x, c.y, Rect::Align::CENTER);
                GameObjects::Get<VelocityComponent>()[mProjs->back()->id()].set(
                    {v.x / mag, v.y / mag});
            }
        },
        EventSystem::Update);
}

// TODO: ComponentGroup w/ (1) connected active, (2) handle init
// HealthData
HealthData::HealthData(int hp, Rect rect)
    : Components::DataComponent<int>(hp),
      mRect(rect),
      mImgs{"res/projectiles/fireball2.png"} {
    init();
}

void HealthData::operator++() {
    ++mT;
    update();
}
void HealthData::operator--() {
    --mT;
    a = (a + 1) % 9;
    update();
}
void HealthData::operator=(int hp) {
    mT = hp;
    update();
}

void HealthData::init() {
    addComponent<PositionComponent>(mRect);
    addComponent<ElevationComponent>(4);
    addComponent<SpriteComponent>("res/projectiles/fireball.png");
    addComponent<RenderService>();

    update();
}

void HealthData::update() {
    int aX = a % 3, aY = (int)(a / 3);
    Rect::Align alignX, alignY;
    alignX = aX == 0   ? Rect::Align::TOP_LEFT
             : aX == 1 ? Rect::Align::CENTER
                       : Rect::Align::BOT_RIGHT;
    alignY = aY == 0   ? Rect::Align::TOP_LEFT
             : aY == 1 ? Rect::Align::CENTER
                       : Rect::Align::BOT_RIGHT;
    addComponent<TextComponent>(
        std::to_string(mT) + "{i}", mImgs, mRect,
        AssetManager::getFont({-1, (int)(mRect.h() / 2), "|"}), alignX, alignY);
}
