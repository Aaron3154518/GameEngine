#ifndef CAMERA_H
#define CAMERA_H

#include <Entities/Entity.h>
#include <Framework/FrameworkComponents.h>
#include <Framework/RenderSystem/RenderSystem.h>

class Camera : public Entities::Entity {
   public:
    static Camera& Get();

    static Rect& GetRect();

    static Rect GetRelativeRect(Rect r);

   private:
    void init();
};

#endif
