#include "ResizeService.h"

#include "../Game.h"
#include "../GameStruct.h"

ResizeService::ResizeService() {
    Game::registerComponent(this);
}

void ResizeService::init(GameStruct &gs) {
    gs.mServices.eventService.event$.subscribe(
        [this](const Event &e) {
            if (e.resized()) {
                resize$.next(e.newDim());
            }
        });
}