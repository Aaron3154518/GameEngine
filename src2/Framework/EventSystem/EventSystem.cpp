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
    if (!(ss >> key)) {
        return nullptr;
    }
    return std::make_unique<Msg>(
        code, Event::KeyButton{static_cast<SDL_KeyCode>(key), 0, code});
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
    mb.sendMessage(EventMessage(OnEvent, mEvent, {Entities::NullId(), true}));

    sendKeyMessages();
}

void EventSystem::runNextFrame(const Func& func) {
    GetQueued().push_back(std::move(func));
}

void EventSystem::runQueued() {
    for (auto& func : GetQueued()) {
        func();
    }
    GetQueued().clear();
}

void EventSystem::sendKeyMessages() {
    auto& mb = Messages::GetMessageBus();
    static const auto codes = {Event::DOWN, Event::PRESSED, Event::UP,
                               Event::HELD};
    for (auto code : codes) {
        for (auto& pair : mEvent.getKeys()) {
            auto& kb = pair.second;
            if (Math::allBitsSet(kb.status, code)) {
                mb.sendMessage(KeyboardMessage(
                    code, kb,
                    {Entities::NullId(), /*code == Event::HELD*/ true}));
            }
        }
    }
}

bool EventSystem::sendDragMessages() { return false; }

void EventSystem::sendHoverMessages() {}

std::vector<EventSystem::Func>& EventSystem::GetQueued() {
    static std::vector<Func> QUEUED;
    return QUEUED;
}
