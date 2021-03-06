#include "Time.h"

Time::Time(uint32_t timeMs) : mTimeMs(timeMs) {}

float Time::s() const {
    return mTimeMs / 1000.0f;
}
uint32_t Time::ms() const {
    return mTimeMs;
}

Time::operator uint32_t() const {
    return mTimeMs;
}

void Time::operator=(const uint32_t &other) {
    mTimeMs = other;
}

Time &Time::operator+=(const uint32_t &rhs) {
    mTimeMs += rhs;
    return *this;
}
Time &Time::operator+=(const Time &rhs) { return (*this += rhs.mTimeMs); }