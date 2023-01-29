#include "EventSystem.h"

// Messages
REGISTER(EventSystem::UpdateMessage, UpdateMessage, {
    std::stringstream ss(line);
    int dt;
    return std::make_unique<Msg>(code, ss >> dt ? std::max(0, dt) : 1.0 / 60.0);
});
REGISTER(EventSystem::KeyboardMessage, KeyboardMessage, {
    std::stringstream ss(line);
    char key;
    if (code < 0) {
        if (!(ss >> key)) {
            return nullptr;
        }
        code = static_cast<SDL_KeyCode>(key);
    }
    uint8_t action;
    action = ss >> action ? std::min(action, Event::KEY_ALL) : Event::KEY_ALL;
    return std::make_unique<Msg>(code, Event::KeyButton{code, 0, action});
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
    mb.sendMessage(UpdateMessage(Update, dt, {Entities::NullId(), true}));
    for (auto& kb : mEvent.keys([](const Event::KeyButton& kb) {
             return Math::anyBitsSet(kb.status, Event::KEY_ALL);
         })) {
        mb.sendMessage(KeyboardMessage(kb.key, kb));
    }
}
