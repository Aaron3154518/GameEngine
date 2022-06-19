#ifndef SERVICE_HANDLER_H
#define SERVICE_HANDLER_H

#include "MouseService/MouseService.h"
#include "EventService/EventService.h"

class ServiceHandler
{
public:
    ServiceHandler() = default;
    ~ServiceHandler() = default;

    EventService eventService;
    MouseService mouseService;
};

#endif
