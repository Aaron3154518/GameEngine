#include "Event.h"

// MouseButton
bool Event::MouseButton::down() const {
    return Math::allBitsSet(status, Event::Status::DOWN);
}
bool Event::MouseButton::up() const {
    return Math::allBitsSet(status, Event::Status::UP);
}
bool Event::MouseButton::held() const {
    return Math::allBitsSet(status, Event::Status::HELD);
}
bool Event::MouseButton::clicked() const {
    return Math::allBitsSet(status, Event::Status::CLICKED);
}

// KeyButton
bool Event::KeyButton::down() const {
    return Math::allBitsSet(status, Event::Status::DOWN);
}
bool Event::KeyButton::up() const {
    return Math::allBitsSet(status, Event::Status::UP);
}
bool Event::KeyButton::held() const {
    return Math::allBitsSet(status, Event::Status::HELD);
}
bool Event::KeyButton::pressed() const {
    return Math::allBitsSet(status, Event::Status::PRESSED);
}

// Event
Event::Event() {
    for (auto btn : {Mouse::LEFT, Mouse::RIGHT, Mouse::MIDDLE}) {
        mMouseButtons[btn].mouse = btn;
    }
}

void Event::update(uint32_t ts, const Rect &camera, Dimensions screen) {
    dt = ts;
    // Reset event flags
    quit = resized = false;
    // Reset text editing
    inputText = "";
    inputBackspace = inputDelete = inputMove = 0;
    inputSeek = InputSeek::NONE;
    // Update mouse
    SDL_GetMouseState(&absMouse.x, &absMouse.y);
    mouse = {(int)(absMouse.x * camera.w() / screen.w + camera.x()),
             (int)(absMouse.y * camera.h() / screen.h + camera.y())};
    // Reset mouse movement
    mouseDelta = {0, 0};
    scroll = 0;
    // Update mouse buttons
    for (auto &b : mMouseButtons) {
        if (b.held()) {
            b.duration += ts;
        }
        // Reset pressed/released
        b.status &= Status::HELD;
    }
    // Update keys
    for (auto it = mKeyButtons.begin(); it != mKeyButtons.end(); ++it) {
        auto &b = it->second;
        if (b.held()) {
            b.duration += ts;
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
            b.status = Status::DOWN | Status::HELD;
            b.duration = 0;
            b.clickPos = mouse;
        } break;
        case SDL_MOUSEBUTTONUP: {
            MouseButton &b = mMouseButtons[toMouse(e.button.button)];
            b.status = (Math::distance(b.clickPos, mouse) < MAX_CLICK_DIFF
                            ? Status::CLICKED | Status::UP
                            : Status::UP);
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
            bool held = b.held();
            b.status = Status::PRESSED | Status::HELD;
            if (!held) {
                b.status |= Status::DOWN;
                b.duration = 0;
            }
            if (SDL_IsTextInputActive()) {
                processTextInputKey(b);
            }
        } break;
        case SDL_KEYUP: {
            KeyButton &b = get((SDL_KeyCode)e.key.keysym.sym);
            b.status = Status::UP;
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
    return mKeyButtons.emplace(key, KeyButton{key}).first->second;
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
const std::array<Event::MouseButton, NUM_MICE> &Event::getMice() const {
    return mMouseButtons;
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
const std::unordered_map<SDL_KeyCode, Event::KeyButton> &Event::getKeys()
    const {
    return mKeyButtons;
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
