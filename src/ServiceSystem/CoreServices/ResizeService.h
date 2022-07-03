#ifndef RESIZE_SERVICE_H
#define RESIZE_SERVICE_H

#include <SDL.h>

#include "ServiceSystem/CoreServices/EventService.h"
#include "ServiceSystem/Observable.h"
#include "ServiceSystem/Service.h"

typedef Observable<SDL_Point, void(SDL_Point)> ResizeObservable;

class ResizeService : public Service<ResizeObservable> {};

#endif