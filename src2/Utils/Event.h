#ifndef EVENT_H
#define EVENT_H

#include <SDL.h>
#include <Utils/Math.h>
#include <Utils/Rect.h>

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

    enum Status : uint8_t {
        DOWN = 0x01,
        UP = 0x02,
        HELD = 0x04,
        CLICKED = 0x08,  // For mouse buttons
        PRESSED = 0x08   // For key buttons
    };

    struct MouseButton {
        Mouse mouse;
        SDL_Point clickPos{0, 0};
        uint32_t duration;
        uint8_t status = 0;

        bool down() const;
        bool up() const;
        bool held() const;
        bool clicked() const;
    };
    struct KeyButton {
        SDL_KeyCode key;
        uint32_t duration;
        uint8_t status = 0;

        bool down() const;
        bool up() const;
        bool held() const;
        bool pressed() const;
    };

    void update(uint32_t dt);

    bool quit = false;
    // Resize data
    bool resized = false;
    Dimensions oldDim, newDim;

    // Mouse position and movement
    SDL_Point mouse;
    SDL_Point mouseDelta;
    bool mouseMoved() const;
    // < 0 = down, > 0 = up
    int scroll = 0;

    // Inputted text
    enum InputSeek : uint8_t { NONE = 0, START, END };
    std::string inputText;
    int inputBackspace = 0;
    int inputDelete = 0;
    int inputMove = 0;
    InputSeek inputSeek = InputSeek::NONE;

    KeyButton &get(SDL_KeyCode key);

    const MouseButton &operator[](Uint8 sdlButton) const;
    const MouseButton &operator[](Event::Mouse button) const;
    const KeyButton &operator[](SDL_KeyCode key) const;
    const std::array<MouseButton, NUM_MICE> &getMice() const;
    std::vector<KeyButton> keys(std::function<bool(const KeyButton &)> filter =
                                    [](const KeyButton &k) {
                                        return true;
                                    }) const;
    const std::unordered_map<SDL_KeyCode, KeyButton> &getKeys() const;
    static const KeyButton &unusedKey();

    static Mouse toMouse(Uint8 sdlButtonType);

   private:
    static constexpr int MAX_CLICK_DIFF = 10;

    void update(SDL_Event &e);

    void processTextInputKey(const KeyButton &b);

    std::array<MouseButton, NUM_MICE> mMouseButtons;
    std::unordered_map<SDL_KeyCode, KeyButton> mKeyButtons;
};

#endif