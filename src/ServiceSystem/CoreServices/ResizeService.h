#ifndef RESIZE_SERVICE_H
#define RESIZE_SERVICE_H

#include <SDL.h>

#include "../../Utils/Event.h"
#include "../Component.h"
#include "../Observable.h"
#include "../ServiceSystem.h"

typedef Observable<SDL_Point, void(SDL_Point)> ResizeObservable;

class ResizeService : public Service<ResizeObservable> {};

#endif