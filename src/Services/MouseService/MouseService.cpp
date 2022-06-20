#include "MouseService.h"
#include "../GameStruct.h"

void MouseService::init(GameStruct &gs)
{
    gs.mServices.eventService.event$.subscribe([this](const Event &e)
                                               {
                                                std::cerr << "Event" << std::endl;
        if (e[Event::LEFT].clicked()) {
            mouse$.next(e[Event::LEFT]);
        } std::cerr << "End Event" << std::endl; });
}