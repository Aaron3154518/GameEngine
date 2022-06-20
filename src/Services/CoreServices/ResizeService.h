#ifndef RESIZE_SERVICE_H
#define RESIZE_SERVICE_H

#include <SDL.h>

#include "../../EventSystem/Event.h"
#include "../../Utils/Observable/Observable.h"
#include "../Component.h"

class ResizeService : public Component {
   public:
    ResizeService();
    ~ResizeService() = default;

    Observable<SDL_Point, void(SDL_Point)> resize$;

   private:
    void init(GameStruct &gs);
};

#endif