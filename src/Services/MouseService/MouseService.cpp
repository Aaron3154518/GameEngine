#include "MouseService.h"
#include "../GameStruct.h"

void MouseService::init(GameStruct &gs)
{
    gs.mServices.eventService.event$.subscribe([this](const Event &e)
                                               {
        if (e[Event::LEFT].clicked()) {
            mouse$.next(e[Event::LEFT]);
        } });
}