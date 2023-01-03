#include "Event.h"

// MouseButton
bool Event::MouseButton::pressed() const {
    return Math::bitsSet(status, Event::Button::PRESSED);
}
bool Event::MouseButton::released() const {
    return Math::bitsSet(status, Event::Button::RELEASED);
}
bool Event::MouseButton::held() const {
    return Math::bitsSet(status, Event::Button::HELD);
}
bool Event::MouseButton::clicked() const {
    return Math::bitsSet(status, Event::Button::CLICKED);
}

// KeyButton
bool Event::KeyButton::pressed() const {
    return Math::bitsSet(status, Event::Button::PRESSED);
}
bool Event::KeyButton::released() const {
    return Math::bitsSet(status, Event::Button::RELEASED);
}
bool Event::KeyButton::held() const {
    return Math::bitsSet(status, Event::Button::HELD);
}

// Event
Event::Event() {
    for (auto btn : {Mouse::LEFT, Mouse::RIGHT, Mouse::MIDDLE}) {
        mMouseButtons[btn].mouse = btn;
    }
}

void Event::update() {
    static uint32_t time = 0;
    // Reset/update variables
    mDt = time == 0 ? 0 : SDL_GetTicks() - time;
    time = SDL_GetTicks();
    // Reset event flags
    mQuit = mResized = false;
    // Reset text editing
    mInputText = "";
    // Update mouse
    SDL_GetMouseState(&mMouse.x, &mMouse.y);
    // Reset mouse movement
    mMouseDx = mMouseDy = 0;
    mScroll = 0;
    // Update mouse buttons
    for (auto &b : mMouseButtons) {
        if (Math::bitsSet(b.status, Button::HELD)) {
            b.duration += mDt;
        }
        // Reset pressed/released
        b.status &= Button::HELD;
    }
    // Update keys
    for (auto it = mKeyButtons.begin(); it != mKeyButtons.end(); ++it) {
        auto &b = it->second;
        if (Math::bitsSet(b.status, Button::HELD)) {
            b.duration += mDt;
        }
        // Reset pressed/released
        b.status &= Button::HELD;
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
            mQuit = true;
            break;
        }
        case SDL_WINDOWEVENT: {
            switch (e.window.event) {
                case SDL_WINDOWEVENT_SHOWN:
                    SDL_GetWindowSize(SDL_GetWindowFromID(e.window.windowID),
                                      &mOldW, &mOldH);
                    mNewW = mOldW;
                    mNewH = mOldH;
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    mResized = true;
                    mOldW = mNewW;
                    mOldH = mNewH;
                    mNewW = e.window.data1;
                    mNewH = e.window.data2;
                    break;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            MouseButton &b = mMouseButtons[toMouse(e.button.button)];
            b.status = Button::PRESSED | Button::HELD;
            b.duration = 0;
            b.clickPos = mMouse;
        } break;
        case SDL_MOUSEBUTTONUP: {
            MouseButton &b = mMouseButtons[toMouse(e.button.button)];
            b.status = (Math::distance(b.clickPos, mMouse) < MAX_CLICK_DIFF
                            ? Button::CLICKED
                            : 0) |
                       Button::RELEASED;
            b.duration = 0;
        } break;
        case SDL_MOUSEMOTION: {
            mMouseDx = e.motion.xrel;
            mMouseDy = e.motion.yrel;
        } break;
        case SDL_MOUSEWHEEL: {
            mScroll = -e.wheel.y;
        } break;
        case SDL_KEYDOWN: {
            KeyButton &b = get((SDL_KeyCode)e.key.keysym.sym);
            if (!Math::bitsSet(b.status, Button::HELD)) {
                b.duration = 0;
            }
            b.status = Button::PRESSED | Button::HELD;
        } break;
        case SDL_KEYUP: {
            KeyButton &b = get((SDL_KeyCode)e.key.keysym.sym);
            b.status = Button::RELEASED;
        } break;
        case SDL_TEXTEDITING:
            break;
        case SDL_TEXTINPUT: {
            mInputText = e.text.text;
        } break;
        default:
#ifdef EVENT_DEBUG
            std::cerr << "Unhandled Event" << std::endl;
#endif
            break;
    }
}

uint32_t Event::dt() const { return mDt; }

bool Event::quit() const { return mQuit; }
bool Event::resized() const { return mResized; }
SDL_Point Event::oldDim() const { return SDL_Point{mOldW, mOldH}; }
int Event::oldW() const { return mOldW; }
int Event::oldH() const { return mOldH; }
SDL_Point Event::newDim() const { return SDL_Point{mNewW, mNewH}; }
int Event::newW() const { return mNewW; }
int Event::newH() const { return mNewH; }

SDL_Point Event::mouse() const { return mMouse; }
float Event::mouseDx() const { return mMouseDx; }
float Event::mouseDy() const { return mMouseDy; }
bool Event::mouseMoved() const { return mMouseDx != 0 || mMouseDy != 0; }
int Event::scroll() const { return mScroll; }

std::string Event::textInput() const { return mInputText; }

Event::KeyButton &Event::get(SDL_KeyCode key) {
    return mKeyButtons.emplace(key, KeyButton{key}).first->second;
}

const Event::MouseButton &Event::operator[](Uint8 sdlButton) const {
    return (*this)[toMouse(sdlButton)];
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
        default:
            return Event::Mouse::LEFT;
    }
}