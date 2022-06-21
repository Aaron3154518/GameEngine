#ifndef RESIZE_SERVICE_H
#define RESIZE_SERVICE_H

#include <SDL.h>

#include "../../EventSystem/Event.h"
#include "../../Utils/Observable/Observable.h"
#include "../Component.h"
#include "../Game.h"
#include "../GameStruct.h"
#include "../ServiceHandler.h"

class ResizeService : public Service, public Component {
   public:
    ResizeService();
    ~ResizeService() = default;

    Observable<SDL_Point, void(SDL_Point)> resize$;

   private:
    void init(GameStruct &gs);
};

REGISTER_SERVICE(ResizeService);

#endif