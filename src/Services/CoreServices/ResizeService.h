#ifndef RESIZE_SERVICE_H
#define RESIZE_SERVICE_H

#include <SDL.h>

#include "../../EventSystem/Event.h"
#include "../../Utils/Observable/Observable.h"
#include "../Component.h"
#include "../ServiceHandler.h"

typedef Observable<SDL_Point, void(SDL_Point)> ResizeObservable;

class ResizeService : public Service<ResizeObservable> {};

#endif