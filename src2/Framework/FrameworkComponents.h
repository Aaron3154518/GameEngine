#ifndef FRAMEWORK_COMPONENTS_H
#define FRAMEWORK_COMPONENTS_H

#include <Components/ComponentManager.h>
#include <Utils/Rect.h>

class PositionComponent : public Components::DataComponent<Rect> {
   public:
    using Components::DataComponent<Rect>::DataComponent;
};
typedef Components::ComponentManager<PositionComponent>
    PositionComponentManager;

class VelComponent : public Components::DataComponent<SDL_FPoint> {
   public:
    using Components::DataComponent<SDL_FPoint>::DataComponent;
};
typedef Components::ComponentManager<VelComponent> VelComponentManager;

class AccelComponent : public Components::DataComponent<SDL_FPoint> {
   public:
    using Components::DataComponent<SDL_FPoint>::DataComponent;
};
typedef Components::ComponentManager<AccelComponent> AccelComponentManager;

class ElevationComponent : public Components::DataComponent<int> {
   public:
    using Components::DataComponent<int>::DataComponent;
};
typedef Components::ComponentManager<ElevationComponent>
    ElevationComponentManager;

#endif
