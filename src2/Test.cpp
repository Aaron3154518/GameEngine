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
    addComponent<Components::ComponentManager<Components::Component>>();
    addComponent<MyComponentManager>();
    subscribeTo<MyService::Message>(
        [](const MyService::Message& m) {
            Messages::GetMessageBus().sendMessage(
                MyService::Message(MyService::World));
        },
        MyService::Hello);

    addComponent<PositionComponentManager>(Rect(10, 10, 50, 50));
    addComponent<VelComponentManager>(SDL_FPoint{0, 0});
    addComponent<BoundaryComponentManager>(BOUND);
    addComponent<PhysicsService>();

    addComponent<ElevationComponentManager>(1);
    addComponent<SpriteComponentManager>("res/wizards/wizard_ss.png", 5, 150);
    addComponent<RenderService>();

    addComponent<CollisionComponentManager>(P);
    addComponent<CollisionService>();
    subscribeTo<CollisionService::Message>(
        [this](const CollisionService::Message& m) {
            if (!immune) {
                hp--;
                if (hp <= 0) {
                    hp = 5;
                    getComponent<PositionComponentManager>().get() =
                        Rect(10, 10, 50, 50);
                }
                immune = true;
                startTimer(2000, [this]() { immune = false; });
            }
        },
        CollisionService::Collided);

    subscribeTo<EventSystem::KeyboardMessage>(
        [this](const EventSystem::KeyboardMessage& m) {
            auto& pos = getComponent<PositionComponentManager>().get();
            switch (m.data.key) {
                case SDLK_r:
                    pos = Rect(10, 10, 50, 50);
                    break;
                default:
                    break;
            }
        },
        Event::PRESSED);
    static int V = 100;
    subscribeTo<EventSystem::KeyboardMessage>(
        [this](const EventSystem::KeyboardMessage& m) {
            auto& v = getComponent<VelComponentManager>().get();
            switch (m.data.key) {
                case SDLK_a:
                    v.x = -V;
                    break;
                case SDLK_d:
                    v.x = V;
                    break;
                case SDLK_w:
                    v.y = -V;
                    break;
                case SDLK_s:
                    v.y = V;
                    break;
                default:
                    break;
            }
        },
        Event::HELD);
    subscribeTo<EventSystem::KeyboardMessage>(
        [this](const EventSystem::KeyboardMessage& m) {
            auto& v = getComponent<VelComponentManager>().get();
            switch (m.data.key) {
                case SDLK_a:
                case SDLK_d:
                    v.x = 0;
                    break;
                case SDLK_w:
                case SDLK_s:
                    v.y = 0;
                    break;
                default:
                    break;
            }
        },
        Event::RELEASED);
    subscribeTo<EventSystem::MouseMessage>(
        [this](const EventSystem::MouseMessage& m) {
            if (m.data.mouse == Event::LEFT) {
                getComponent<PositionComponentManager>().get().setPos(
                    m.data.clickPos.x, m.data.clickPos.y, Rect::Align::CENTER);
            }
        },
        Event::CLICKED);
}

// EntityProj
void EnemyProj::init() {
    addComponent<PositionComponentManager>(Rect(0, 0, 20, 20));
    addComponent<VelComponentManager>(SDL_FPoint{0, 0});
    addComponent<PhysicsService>();

    addComponent<ElevationComponentManager>(2);
    addComponent<SpriteComponentManager>("res/wizards/catalyst.png");
    addComponent<RenderService>();

    addComponent<CollisionComponentManager>(E);
    addComponent<CollisionService>();
    subscribeTo<CollisionService::Message>(
        [this](const auto& m) {
            Messages::GetMessageBus().sendMessage(
                EnemyProjCont::Message(EnemyProjCont::Remove, id()));
        },
        CollisionService::Collided);
    subscribeTo<EventSystem::UpdateMessage>(
        [this](const auto& m) {
            auto& pos = getComponent<PositionComponentManager>().get();
            SDL_Rect _;
            if (!SDL_IntersectRect(pos, BOUND, &_)) {
                Messages::GetMessageBus().sendMessage(EnemyProjCont::Message(
                    EnemyProjCont::Remove, id(), {Entities::NullId(), true}));
            }
        },
        EventSystem::Update);
}
