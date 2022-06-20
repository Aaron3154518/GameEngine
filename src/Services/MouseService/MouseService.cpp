#include "MouseService.h"
#include "../Game.h"
#include "../GameStruct.h"

MouseService::MouseService()
{
    Game::registerComponent(this);
}

void MouseService::init(GameStruct &gs)
{
    gs.mServices.eventService.event$.subscribe(
        [this](const Event &e)
        {
            if (e[Event::LEFT].clicked())
            {
                mouse$.next(e[Event::LEFT]);
            }
        });
}