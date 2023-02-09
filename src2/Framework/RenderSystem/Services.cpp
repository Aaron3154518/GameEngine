#include "Services.h"

REGISTER(RenderService::Message, RenderMessage);

void RenderService::manager_init() {
    subscribeTo<Message>([this](const auto& m) { render(); }, Render);
}

void RenderService::render() {
    std::vector<Components::EntityComponents> entities;
    for (auto e :
         require<ElevationComponent, PositionComponent, SpriteComponent>()) {
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
    for (auto& e : entities) {
        draw(e.get<SpriteComponent>(), e.getData<PositionComponent>());
    }
    RenderSystem::presentScreen();
}

void RenderService::draw(const SpriteData& sprite,
                         const Rect& rect) {  // Check the texture to draw
    if (!sprite.mTex) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Invalid Texture" << std::endl;
#endif
        return;
    }

    Dimensions texDim = sprite.getTextureDim();

    // Rect dest = getDest();

    Rect area((sprite.mArea.x() + sprite.mFrame) * texDim.w,
              sprite.mArea.y() * texDim.h, sprite.mArea.w() * texDim.w,
              sprite.mArea.h() * texDim.h);

    // Make sure we are actually drawing something
    if (rect.empty() || area.empty()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Empty destination rect" << std::endl;
#endif
        return;
    }

    int w, h;
    Renderer::getTargetSize(&w, &h);
    Rect renderBounds(0, 0, w, h);

    // Get the boundary rect
    Rect boundary = sprite.mBounds;
    SDL_Rect result;
    if (boundary.empty()) {
        boundary = renderBounds;
    } else {
        if (SDL_IntersectRect(boundary, renderBounds, &result) == SDL_FALSE) {
#ifdef RENDER_DEBUG
            std::cerr << "draw(): Boundary rect " << boundary
                      << " was outside the screen: " << renderBounds
                      << std::endl;
#endif
            return;
        }
        boundary = result;
    }

    // Get fraction of item to draw
    float leftFrac = fmaxf(boundary.x() - rect.x(), 0) / rect.w();
    float topFrac = fmaxf(boundary.y() - rect.y(), 0) / rect.h();
    float rightFrac = fmaxf(rect.x2() - boundary.x2(), 0) / rect.w();
    float botFrac = fmaxf(rect.y2() - boundary.y2(), 0) / rect.h();

    Rect dest(rect.x() + rect.w() * leftFrac, rect.y() + rect.h() * topFrac,
              rect.w() * (1. - leftFrac - rightFrac),
              rect.h() * (1. - topFrac - botFrac));

    // Make sure the rect is actually in the boundary
    if (leftFrac + rightFrac >= 1 || topFrac + botFrac >= 1) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Rect " << dest << " was out side the boundary "
                  << boundary << std::endl;
#endif
        return;
    }

    area = Rect(
        area.x() + area.w() * leftFrac,  // + texDim.x * mTex->getFrame(),
        area.y() + area.h() * topFrac, area.w() * (1.f - leftFrac - rightFrac),
        area.h() * (1.f - topFrac - botFrac));

    // Make sure at least one pixel will be drawn
    if (area.empty()) {
#ifdef RENDER_DEBUG
        std::cerr << "draw(): Can't draw from " << area << " to " << dest
                  << std::endl;
#endif
        return;
    }

    SDL_RenderCopyEx(Renderer::get(), sprite.mTex.get(), area, dest,
                     0,  // mRotation,
                     NULL, SDL_FLIP_NONE);
}
