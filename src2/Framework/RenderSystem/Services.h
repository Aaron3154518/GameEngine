#ifndef RENDER_SERVICES_H
#define RENDER_SERVICES_H

#include <Entities/Camera.h>
#include <Framework/FrameworkComponents.h>
#include <Framework/RenderSystem/AssetManager.h>
#include <Framework/RenderSystem/RenderSystem.h>
#include <Framework/RenderSystem/Renderer.h>
#include <Framework/RenderSystem/SpriteComponent.h>
#include <Messages/MessageBus.h>
#include <Services/Service.h>

class RenderService : public Services::Service<> {
   public:
    MESSAGE(Message, Render);

   private:
    void manager_init();

    void render();
};

#endif
