#ifndef TIME_H
#define TIME_H

#include <SDL.h>

class Time {
   public:
    Time(uint32_t timeMs = 0);
    ~Time() = default;

    operator uint32_t() const;

    float s() const;
    uint32_t ms() const;

    Time &operator+=(const uint32_t &rhs);
    Time &operator+=(const Time &rhs);
    Time &operator-=(const uint32_t &rhs);
    Time &operator-=(const Time &rhs);
    Time &operator*=(const uint32_t &rhs);
    Time &operator*=(const Time &rhs);
    Time &operator/=(const uint32_t &rhs);
    Time &operator/=(const Time &rhs);

   private:
    uint32_t mTimeMs;
};

#endif