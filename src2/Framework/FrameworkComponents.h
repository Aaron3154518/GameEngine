#ifndef FRAMEWORK_COMPONENTS_H
#define FRAMEWORK_COMPONENTS_H

#include <Components/ComponentManager.h>
#include <Utils/Rect.h>

class PositionComponent
    : public Components::ComponentManager<Components::DataComponent<Rect>> {};

class ElevationComponent
    : public Components::ComponentManager<Components::DataComponent<int>> {};

#endif
