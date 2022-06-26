#include "ResizeService.h"

#include "EventService.h"

ResizeService::ResizeService() {
    Game::registerComponent(this);
}

void ResizeService::init() {
    ServiceHandler::Get<EventService>()->event$.subscribe(
        [this](const Event &e) {
            if (e.resized()) {
                resize$.next(e.newDim());
            }
        });
}