#ifndef MOUSE_SERVICE_H
#define MOUSE_SERVICE_H

#include "../Component.h"
#include "MouseObservable.h"

// Forward declarations
struct GameStruct;

class MouseService : public Component
{
public:
    MouseService() = default;
    ~MouseService() = default;

    void init(GameStruct &gs);

    MouseObservable mouse$;
};

#endif
