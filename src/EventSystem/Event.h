#ifndef EVENT_H
#define EVENT_H

#include <array>
#include <iostream>
#include <cmath>
#include <unordered_map>

#include <SDL.h>

#include "Time.h"

class Event {
public:
	Event() = default;
	~Event() = default;

    typedef uint8_t Status;
    typedef SDL_KeyCode Key;
    enum Mouse : uint8_t {
        LEFT = 0, RIGHT, MIDDLE
    };

    enum Access : Status {
        IGNORED = 0, // Handled status is ignored
        ACTIVE, // Currently handling (taking events)
        INACTIVE, // Not handled and not currently handling
        HANDLED // Has been handled
    };
    enum Button : Status {
        PRESSED = 0x01,
        RELEASED = 0x02,
        HELD = 0x04,

        CLICKED = 0x08, // For mouse buttons
    };

    struct MouseButton {
        Status status = 0;
        SDL_Point clickPos{ 0,0 };
        Time duration;

        bool pressed() const;
        bool released() const;
        bool held() const;
        bool clicked() const;
    };
    struct KeyButton {
        Status status = 0;
        Time duration;

        bool pressed() const;
        bool released() const;
        bool held() const;
    };

    void update(Time ts);
    void update(SDL_Event& e);

    // Getters
    const MouseButton& operator[](Uint8 sdlButton) const;
    const MouseButton& operator[](Event::Mouse button) const;
    const KeyButton& operator[](Key key) const;
    static const KeyButton& unusedKey();

    // Doesn't belong in Event
    static bool bitsSet(uint8_t val, uint8_t check);
    static double distance(SDL_Point p1, SDL_Point p2);

    // Vars
    static constexpr int MAX_CLICK_DIFF = 10;

    Time dt;

    bool quit = false;
    bool resize = false;
    int newW = 0, newH = 0;

    SDL_Point mouse;
    double mouseDx = 0., mouseDy = 0.;
    // < 0 = down, > 0 = up
    int scroll = 0;

    // Inputted text
    std::string inputText;

    std::array<MouseButton, 3> mouseButtons;
    std::unordered_map<Key, KeyButton> keyButtons;

    static Mouse toMouse(Uint8 sdlButtonType);
};

#endif