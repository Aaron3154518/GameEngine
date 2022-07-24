#ifndef RANGE_H
#define RANGE_H

#include <cstddef>

// Range
template <size_t...>
struct Range {};

template <bool, size_t, class>
struct Merge;

template <size_t Off, size_t Start, size_t... Tail>
struct Merge<false, Off, Range<Start, Tail...>> {
    typedef Range<Start, Tail..., (Off + Tail)...> range;
};

template <size_t Off, size_t... Idxs>
struct Merge<true, Off, Range<Idxs...>> {
    typedef Range<Idxs..., (Off + Idxs)...> range;
};

template <size_t Start, size_t End>
struct RangeGenImpl {
    static_assert(Start < End, "Invalid start");

    typedef typename Merge<
        (End - Start + 1) % 2 == 0, (End - Start + 1) / 2,
        typename RangeGenImpl<Start, Start + (End - Start) / 2>::range>::range
        range;
};

template <size_t End>
struct RangeGenImpl<End, End> {
    typedef Range<End> range;
};

template <size_t Start, size_t End>
using RangeGen = typename RangeGenImpl<Start, End - 1>::range;

#endif