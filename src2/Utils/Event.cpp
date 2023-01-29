#include "Event.h"

// MouseButton
bool Event::MouseButton::pressed() const {
    return Math::allBitsSet(status, Event::Status::PRESSED);
}
bool Event::MouseButton::released() const {
    return Math::allBitsSet(status, Event::Status::RELEASED);
}
bool Event::MouseButton::held() const {
    return Math::allBitsSet(status, Event::Status::HELD);
}
bool Event::MouseButton::clicked() const {
    return Math::allBitsSet(status, Event::Status::CLICKED);
}

// KeyButton
bool Event::KeyButton::pressed() const {
    return Math::allBitsSet(status, Event::Status::PRESSED);
}
bool Event::KeyButton::released() const {
    return Math::allBitsSet(status, Event::Status::RELEASED);
}
bool Event::KeyButton::held() const {
    return Math::allBitsSet(status, Event::Status::HELD);
}

// Event
Event::Event() {
    for (auto btn : {Mouse::LEFT, Mouse::RIGHT, Mouse::MIDDLE}) {
        mMouseButtons[btn].mouse = btn;
    }
}

void Event::update(uint32_t dt) {
    // Reset event flags
    quit = resized = false;
    // Reset text editing
    inputText = "";
    inputBackspace = inputDelete = inputMove = 0;
    inputSeek = InputSeek::NONE;
    // Update mouse
    SDL_GetMouseState(&mouse.x, &mouse.y);
    // Reset mouse movement
    mouseDelta = {0, 0};
    scroll = 0;
    // Update mouse buttons
    for (auto &b : mMouseButtons) {
        if (b.held()) {
            b.duration += dt;
        }
        // Reset pressed/released
        b.status &= Status::HELD;
    }
    // Update keys
    for (auto it = mKeyButtons.begin(); it != mKeyButtons.end(); ++it) {
        auto &b = it->second;
        if (b.held()) {
            b.duration += dt;
        }
        // Reset pressed/released
        b.status &= Status::HELD;
    }
    // Handle events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        update(e);
    }
}
void Event::update(SDL_Event &e) {
    switch (e.type) {
        case SDL_QUIT: {
#ifdef DEBUG_EVENT
            std::cerr << "Quit" << std::endl;
#endif
            quit = true;
            break;
        }
        case SDL_WINDOWEVENT: {
            switch (e.window.event) {
                case SDL_WINDOWEVENT_SHOWN:
                    SDL_GetWindowSize(SDL_GetWindowFromID(e.window.windowID),
                                      &oldDim.w, &oldDim.h);
                    newDim = oldDim;
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    resized = true;
                    oldDim = newDim;
                    newDim = {e.window.data1, e.window.data2};
                    break;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            MouseButton &b = mMouseButtons[toMouse(e.button.button)];
            b.status = Status::PRESSED | Status::HELD;
            b.duration = 0;
            b.clickPos = mouse;
        } break;
        case SDL_MOUSEBUTTONUP: {
            MouseButton &b = mMouseButtons[toMouse(e.button.button)];
            b.status = (Math::distance(b.clickPos, mouse) < MAX_CLICK_DIFF
                            ? Status::CLICKED
                            : 0) |
                       Status::RELEASED;
            b.duration = 0;
        } break;
        case SDL_MOUSEMOTION: {
            mouseDelta = {e.motion.xrel, e.motion.yrel};
        } break;
        case SDL_MOUSEWHEEL: {
            scroll = -e.wheel.y;
        } break;
        case SDL_KEYDOWN: {
            KeyButton &b = get((SDL_KeyCode)e.key.keysym.sym);
            if (!b.held()) {
                b.duration = 0;
            }
            b.status = Status::PRESSED | Status::HELD;
            if (SDL_IsTextInputActive()) {
                processTextInputKey(b);
            }
        } break;
        case SDL_KEYUP: {
            KeyButton &b = get((SDL_KeyCode)e.key.keysym.sym);
            b.status = Status::RELEASED;
        } break;
        case SDL_TEXTEDITING:
            break;
        case SDL_TEXTINPUT: {
            inputText += e.text.text;
        } break;
        default:
#ifdef EVENT_DEBUG
            std::cerr << "Unhandled Event" << std::endl;
#endif
            break;
    }
}
void Event::processTextInputKey(const KeyButton &b) {
    switch (b.key) {
        case SDLK_BACKSPACE: {
            if (inputText.empty()) {
                inputBackspace++;
            } else {
                inputText = inputText.substr(0, inputText.size() - 1);
            }
        } break;
        case SDLK_DELETE: {
            inputDelete++;
        } break;
        case SDLK_LEFT: {
            inputMove--;
        } break;
        case SDLK_RIGHT: {
            inputMove++;
        } break;
        case SDLK_HOME: {
            inputSeek = InputSeek::START;
        } break;
        case SDLK_END: {
            inputSeek = InputSeek::END;
        } break;
        default:
            break;
    };
}

bool Event::mouseMoved() const {
    return mouseDelta.x != 0 || mouseDelta.y != 0;
}

Event::KeyButton &Event::get(SDL_KeyCode key) {
    KeyButton kb;
    kb.key = key;
    return mKeyButtons.emplace(key, kb).first->second;
}

const Event::MouseButton &Event::operator[](Event::Mouse button) const {
    return mMouseButtons[button];
}
const Event::KeyButton &Event::operator[](SDL_KeyCode key) const {
    auto it = mKeyButtons.find(key);
    if (it != mKeyButtons.end()) {
        return it->second;
    } else {
        return unusedKey();
    }
}
std::vector<Event::KeyButton> Event::keys(
    std::function<bool(const KeyButton &)> filter) const {
    std::vector<KeyButton> res;
    for (auto &pair : mKeyButtons) {
        if (filter(pair.second)) {
            res.push_back(pair.second);
        }
    }
    return res;
}
const Event::KeyButton &Event::unusedKey() {
    static KeyButton UNUSED;
    return UNUSED;
}

Event::Mouse Event::toMouse(Uint8 sdlButtonType) {
    switch (sdlButtonType) {
        case SDL_BUTTON_MIDDLE:
            return Event::Mouse::MIDDLE;
        case SDL_BUTTON_RIGHT:
            return Event::Mouse::RIGHT;
        case SDL_BUTTON_LEFT:
            return Event::Mouse::LEFT;
        default:
            break;
    }

    throw std::runtime_error("Event::toMouse(): Invalid SDL button type");
}
