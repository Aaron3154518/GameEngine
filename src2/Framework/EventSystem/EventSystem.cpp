#include "EventSystem.h"

// Messages
REGISTER(EventSystem::UpdateMessage, UpdateMessage, {
    std::stringstream ss(line);
    int cnt;
    return std::make_unique<Msg>(code,
                                 ss >> cnt ? std::max(0, cnt) : 1.0 / 60.0);
});

// EventSystem
Event EventSystem::mEvent;

const Event& EventSystem::get() { return mEvent; }

void EventSystem::update() {
    static uint32_t time = 0;
    // Reset/update variables
    uint32_t dt = time == 0 ? 0 : SDL_GetTicks() - time;
    time = SDL_GetTicks();
    // Parse SDL events
    mEvent.update(dt);

    // Dispatch messages
    auto& mb = Messages::GetMessageBus();
    mb.sendMessage(UpdateMessage(Update, dt));
}
