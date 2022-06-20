#include "Time.h"

Time::Time(uint32_t time) : mTime(time) {}

Time::operator uint32_t() const {
    return mTime;
}

float Time::s() const {
    return mTime / 1000.0f;
}
uint32_t Time::ms() const {
    return mTime;
}

Time &Time::operator+=(const uint32_t &rhs) {
    mTime += rhs;
    return *this;
}
Time &Time::operator+=(const Time &rhs) {
    mTime += rhs.mTime;
    return *this;
}