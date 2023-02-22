#ifndef CAMERA_H
#define CAMERA_H

#include <Entities/Entity.h>
#include <Framework/EventSystem/EventSystem.h>
#include <Framework/FrameworkComponents.h>
#include <Framework/RenderSystem/RenderSystem.h>

class Camera : public Entities::Entity {
   public:
    static Camera& Get();

    Rect& getRect();
    Rect getRelativeRect(Rect r);

    void track(const Entities::UUID& eId = Entities::NullId(),
               float maxSpeed = 0);
    const Entities::UUID& getTracking() const;

   private:
    void init();

    float mSpeed = 0;
    Entities::UUID mTrackId = Entities::NullId();
};

#endif
