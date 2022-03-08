#ifndef EVENT_H
#define EVENT_H

#include <array>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <SDL.h>

class Event {
public:
	Event() = default;
	~Event() = default;

    enum Mouse : uint8_t {
        LEFT = 0, RIGHT, MIDDLE
    };
    enum Button : uint8_t {
        PRESSED = 0x01,
        RELEASED = 0x02,
        HELD = 0x04,
        CLICKED = 0x08 // For mouse buttons
    };

    struct MouseButton {
        uint8_t status = 0;
        SDL_Point clickPos{ 0,0 };
        uint32_t duration;

        bool pressed() const;
        bool released() const;
        bool held() const;
        bool clicked() const;
    };
    struct KeyButton {
        uint8_t status = 0;
        uint32_t duration;

        bool pressed() const;
        bool released() const;
        bool held() const;
    };

    void update();
    void update(SDL_Event& e);

    // Getters
    uint32_t dt() const;

    bool quit() const;
    bool resized() const;
    SDL_Point newDim() const;
    int newW() const;
    int newH() const;

    SDL_Point mouse() const;
    double mouseDx() const;
    double mouseDy() const;
    int scroll() const;

    std::string textInput() const;

    const MouseButton& operator[](Uint8 sdlButton) const;
    const MouseButton& operator[](Event::Mouse button) const;
    const KeyButton& operator[](SDL_KeyCode key) const;
    std::vector<SDL_KeyCode> keys() const;
    static const KeyButton& unusedKey();

    static Mouse toMouse(Uint8 sdlButtonType);

    // Don't belong in Event
    static bool bitsSet(uint8_t val, uint8_t check);
    static double distance(SDL_Point p1, SDL_Point p2);

private:
    static constexpr int MAX_CLICK_DIFF = 10;

    uint32_t mDt;

    bool mQuit = false;
    bool mResized = false;
    int mNewW = 0, mNewH = 0;

    SDL_Point mMouse;
    double mMouseDx = 0., mMouseDy = 0.;
    // < 0 = down, > 0 = up
    int mScroll = 0;

    // Inputted text
    std::string mInputText;

    std::array<MouseButton, 3> mMouseButtons;
    std::unordered_map<SDL_KeyCode, KeyButton> mKeyButtons;
};

#endif