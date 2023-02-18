#include "DragService.h"

// MouseOptions
MouseOptions::MouseOptions(bool drag) : canDrag(drag) {}

// MouseService
void MouseService::manager_init() {
    subscribeTo<EventSystem::EventMessage>(
        [this](const EventSystem::EventMessage& m) { onEvent(m.data); });
}

void MouseService::onEvent(const Event& e) {
    static const auto codes = {Event::DOWN, Event::CLICKED, Event::UP,
                               Event::HELD};
    auto& mb = Messages::GetMessageBus();
    auto& mice = e.getMice();

    if (mDragId != Entities::NullId()) {
        // Lost the current drag target
        if (!hasEntity(mDragId) || !operator[](mDragId).canDrag) {
            mDragId = Entities::NullId();
        }

        // Drag
        mb.sendMessage(DragMessage(
            Dragging, {e.mouse.x, e.mouse.y, e.mouseDelta.x, e.mouseDelta.y},
            {mDragId}));

        // Stopped dragging
        for (auto& m : mice) {
            if (m.up()) {
                mb.sendMessage(DragMessage(DragEnd, {}, {mDragId}));
                mDragId = Entities::NullId();
                break;
            }
        }
    }

    if (mDragId != Entities::NullId()) {
        return;
    }

    // Compute mouse target
    std::vector<Components::EntityComponents> entities;
    for (auto e : active<ElevationComponent, PositionComponent>()) {
        entities.push_back(e);
    }

    std::sort(entities.begin(), entities.end(),
              [](const Components::EntityComponents& lhs,
                 const Components::EntityComponents& rhs) {
                  int e1 = lhs.getData<ElevationComponent>();
                  int e2 = rhs.getData<ElevationComponent>();
                  return (e1 > e2) || (e1 == e2 && lhs.id() > rhs.id());
              });

    for (auto& en : entities) {
        auto& pos = en.getData<PositionComponent>();
        if (SDL_PointInRect(&e.mouse, pos)) {
            clicked = true;
            // Start dragging
            if (en.get<MouseService>().canDrag) {
                for (auto& m : mice) {
                    if (m.down()) {
                        mDragId = en.id();
                        mb.sendMessage(DragMessage(DragStart, {}, {mDragId}));
                        break;
                    }
                }
            }
            if (mDragId != Entities::NullId()) {
                break;
            }
            // Mouse clicks
            for (auto code : codes) {
                for (auto& m : mice) {
                    if (Math::allBitsSet(m.status, code)) {
                        mb.sendMessage(
                            MouseMessage(code, m, {Entities::NullId(), true}));
                    }
                }
            }
            break;
        }
    }
}
