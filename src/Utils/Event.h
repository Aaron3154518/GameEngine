#ifndef EVENT_H
#define EVENT_H

#include <SDL.h>
#include <Utils/Math.h>

#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

constexpr int NUM_MICE = 3;

class Event {
   public:
    Event();

    enum Mouse : uint8_t { LEFT = 0, RIGHT, MIDDLE };
    enum Button : uint8_t {
        PRESSED = 0x01,
        RELEASED = 0x02,
        HELD = 0x04,
        CLICKED = 0x08  // For mouse buttons
    };

    struct MouseButton {
        Mouse mouse;
        uint8_t status = 0;
        SDL_Point clickPos{0, 0};
        uint32_t duration;

        bool pressed() const;
        bool released() const;
        bool held() const;
        bool clicked() const;
    };
    struct KeyButton {
        SDL_KeyCode key;
        uint8_t status = 0;
        uint32_t duration;

        bool pressed() const;
        bool released() const;
        bool held() const;
    };

    void update();
    void update(SDL_Event &e);

    // Getters
    uint32_t dt() const;

    bool quit() const;
    bool resized() const;
    SDL_Point oldDim() const;
    int oldW() const;
    int oldH() const;
    SDL_Point newDim() const;
    int newW() const;
    int newH() const;

    SDL_Point mouse() const;
    float mouseDx() const;
    float mouseDy() const;
    bool mouseMoved() const;
    int scroll() const;

    int textInputMove() const;
    int textInputBackspace() const;
    int textInputDelete() const;
    bool textInputSeekStart() const;
    bool textInputSeekEnd() const;
    bool textInputUpdated() const;
    const std::string &textInput() const;

    KeyButton &get(SDL_KeyCode key);

    const MouseButton &operator[](Uint8 sdlButton) const;
    const MouseButton &operator[](Event::Mouse button) const;
    const KeyButton &operator[](SDL_KeyCode key) const;
    std::vector<KeyButton> keys(std::function<bool(const KeyButton &)> filter =
                                    [](const KeyButton &k) {
                                        return true;
                                    }) const;
    static const KeyButton &unusedKey();

    static Mouse toMouse(Uint8 sdlButtonType);

   private:
    static constexpr int MAX_CLICK_DIFF = 10;

    void processTextInputKey(const KeyButton &b);

    uint32_t mDt;

    bool mQuit = false;
    bool mResized = false;
    int mOldW = 0, mOldH = 0;
    int mNewW = 0, mNewH = 0;

    SDL_Point mMouse;
    float mMouseDx = 0., mMouseDy = 0.;
    // < 0 = down, > 0 = up
    int mScroll = 0;

    // Inputted text
    enum InputSeek : uint8_t { NONE = 0, START, END };
    std::string mInputText;
    int mInputBackspace = 0, mInputDelete = 0, mInputMove = 0;
    InputSeek mInputSeek = InputSeek::NONE;

    std::array<MouseButton, NUM_MICE> mMouseButtons;
    std::unordered_map<SDL_KeyCode, KeyButton> mKeyButtons;
};

#endif