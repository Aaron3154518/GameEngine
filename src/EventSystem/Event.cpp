#include "Event.h"

void Event::update(Time ts) {
	// Reset/update variables
	dt = ts;
	uint32_t ms = dt.ms();
	// Reset event flags
	quit = resize = false;
	// Reset text editing
	inputText = "";
	// Update mouse
	SDL_GetMouseState(&mouse.x, &mouse.y);
	// Reset mouse movement
	mouseDx = mouseDy = 0;
	scroll = 0;
	// Update mouse buttons
	for (auto& b : mouseButtons) {
		if (bitsSet(b.status, Button::HELD)) { b.duration += ms; }
		// Reset pressed/released
		b.status &= Button::HELD;
	}
	// Update keys
	for (auto it = keyButtons.begin(); it != keyButtons.end(); ++it) {
		auto& b = it->second;
		if (bitsSet(b.status, Button::HELD)) {
			b.duration += ms;
			// Reset pressed/released
			b.status = Button::HELD;
		} else {
			// Remove key
			it = keyButtons.erase(it);
			if (it == keyButtons.end()) {
				break;
			}
		}
	}
	// Handle events
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
#ifdef DEBUG_EVENT
				std::cerr << "Quit" << std::endl;
#endif
				quit = true;
				break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
					resize = true;
					newW = e.window.data1;
					newH = e.window.data2;
				}
				break;
			default:
				update(e);
				break;
		}
	}
}

void Event::update(SDL_Event& e) {
	switch (e.type) {
		case SDL_MOUSEBUTTONDOWN:
		{
			MouseButton& b = mouseButtons[toMouse(e.button.button)];
			b.status = Button::PRESSED | Button::HELD;
			b.duration = 0;
			b.clickPos = mouse;
		}
		break;
		case SDL_MOUSEBUTTONUP:
		{
			MouseButton& b = mouseButtons[toMouse(e.button.button)];
			b.status = (distance(b.clickPos, mouse) < MAX_CLICK_DIFF ?
				Button::CLICKED : 0) | Button::RELEASED;
			b.duration = 0;
		}
		break;
		case SDL_MOUSEMOTION:
		{
			mouseDx = e.motion.xrel;
			mouseDy = e.motion.yrel;
		}
		break;
		case SDL_MOUSEWHEEL:
		{
			scroll = -e.wheel.y;
		}
		break;
		case SDL_KEYDOWN:
		{
			KeyButton& b = keyButtons[(SDL_KeyCode)e.key.keysym.sym];
			b.status = Button::PRESSED | Button::HELD;
			b.duration = 0;
		}
		break;
		case SDL_KEYUP:
		{
			KeyButton& b = keyButtons[(SDL_KeyCode)e.key.keysym.sym];
			b.status = Button::RELEASED;
		}
		break;
		case SDL_TEXTEDITING:
			break;
		case SDL_TEXTINPUT:
		{
			inputText = e.text.text;
		}
		break;
		default:
#ifdef EVENT_DEBUG
			std::cerr << "Unhandled Event" << std::endl;
#endif
			break;
	}
}

bool Event::MouseButton::pressed() const {
	return bitsSet(status, Event::Button::PRESSED);
}
bool Event::MouseButton::released() const {
	return bitsSet(status, Event::Button::RELEASED);
}
bool Event::MouseButton::held() const {
	return bitsSet(status, Event::Button::HELD);
}
bool Event::MouseButton::clicked() const {
	return bitsSet(status, Event::Button::CLICKED);
}

bool Event::KeyButton::pressed() const {
	return bitsSet(status, Event::Button::PRESSED);
}
bool Event::KeyButton::released() const {
	return bitsSet(status, Event::Button::RELEASED);
}
bool Event::KeyButton::held() const {
	return bitsSet(status, Event::Button::HELD);
}

const Event::MouseButton& Event::operator[](Uint8 sdlButton) const {
	return (*this)[toMouse(sdlButton)];
}
const Event::MouseButton& Event::operator[](Event::Mouse button) const {
	return mouseButtons[button];
}
const Event::KeyButton& Event::operator[](SDL_KeyCode key) const {
	auto it = keyButtons.find(key);
	if (it != keyButtons.end()) {
		return it->second;
	} else {
		return unusedKey();
	}
}

const Event::KeyButton& Event::unusedKey() {
	static KeyButton UNUSED;
	return UNUSED;
}

bool Event::bitsSet(uint8_t val, uint8_t check) {
	return (val & check) == check;
}
double Event::distance(SDL_Point p1, SDL_Point p2) {
	return sqrt((p1.x - p2.x) ^ 2 + (p1.y - p2.y) ^ 2);
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