#ifndef SERVICE_HANDLER_H
#define SERVICE_HANDLER_H

#include "CoreServices/EventService.h"
#include "CoreServices/RenderService.h"
#include "CoreServices/ResizeService.h"
#include "CoreServices/UpdateService.h"
#include "MouseService/MouseService.h"

// TODO: auto index by Service type
// TODO: eliminate classes? Hold observables directly?
class ServiceHandler {
   public:
    ServiceHandler() = default;
    ~ServiceHandler() = default;

    EventService eventService;
    UpdateService updateService;
    RenderService renderService;
    ResizeService resizeService;
    MouseService mouseService;
};

#endif
