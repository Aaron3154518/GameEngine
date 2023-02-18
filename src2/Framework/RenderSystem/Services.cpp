#include "Services.h"

REGISTER(RenderService::Message, RenderMessage);

void RenderService::manager_init() {
    subscribeTo<Message>([this](const auto& m) { render(); }, Render);
}

void RenderService::render() {
    std::vector<Components::EntityComponents> entities;
    for (auto e :
         active<ElevationComponent, PositionComponent, SpriteComponent>()) {
        entities.push_back(e);
    }

    std::sort(entities.begin(), entities.end(),
              [](const Components::EntityComponents& lhs,
                 const Components::EntityComponents& rhs) {
                  int e1 = lhs.getData<ElevationComponent>();
                  int e2 = rhs.getData<ElevationComponent>();
                  return (e1 < e2) || (e1 == e2 && lhs.id() < rhs.id());
              });

    RenderSystem::clearScreen(Colors::White);
    TextureBuilder tex;
    for (auto& e : entities) {
        e.get<SpriteComponent>().draw(
            tex, Camera::GetRelativeRect(e.getData<PositionComponent>()));
    }
    RenderSystem::presentScreen();
}
