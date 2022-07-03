#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

#include <EventSystem/EventSystem.h>
#include <RenderSystem/RenderSystem.h>
#include <SDL.h>
#include <ServiceSystem/CoreServices/EventService.h>
#include <ServiceSystem/CoreServices/RenderService.h>
#include <ServiceSystem/CoreServices/UpdateService.h>
#include <ServiceSystem/ServiceSystem.h>
#include <Utils/Event.h>

namespace GameSystem {
void Init(RenderSystem::Options options);

void Run();

void Clean();
};  // namespace GameSystem

#endif