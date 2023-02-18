#include "Service.h"

namespace Services {
// iterable::iterator
iterable::iterator::iterator(const iterator_base& it, const iterator_base& end,
                             const CheckFunc& check)
    : mIt(it), mEnd(end), mCheck(check) {
    while (mIt != mEnd && !mCheck(mIt.id())) {
        ++mIt;
    }
}

bool iterable::iterator::operator==(const iterator& rhs) const {
    return mIt == rhs.mIt;
}
bool iterable::iterator::operator!=(const iterator& rhs) const {
    return !(*this == rhs);
}

Components::EntityComponents iterable::iterator::operator*() {
    return Components::EntityComponents(mIt.id());
}

iterable::iterator& iterable::iterator::operator++() {
    do {
        ++mIt;
    } while (mIt != mEnd && !mCheck(mIt.id()));
    return *this;
}

iterable::iterator::operator bool() const {
    return mIt != mEnd && mCheck(mIt.id());
}

// iterable
iterable::iterable(const iterator_base& bIt, const iterator_base& eIt,
                   const CheckFunc& check)
    : mBegin(bIt, eIt, check), mEnd(eIt, eIt, check) {}

iterable::iterator iterable::begin() { return mBegin; }
iterable::iterator iterable::end() { return mEnd; }
}  // namespace Services