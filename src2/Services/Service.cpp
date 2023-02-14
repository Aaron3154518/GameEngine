#include "Service.h"

namespace Services {
// Service::iterable::iterator
Service::iterable::iterator::iterator(const iterator_base& it,
                                      const iterator_base& end,
                                      const CheckFunc& func)
    : mIt(it), mEnd(end), mCheck(func) {
    while (mIt != mEnd && !mCheck(mIt.id())) {
        ++mIt;
    }
}

bool Service::iterable::iterator::operator==(const iterator& rhs) const {
    return mIt == rhs.mIt;
}
bool Service::iterable::iterator::operator!=(const iterator& rhs) const {
    return !(*this == rhs);
}

Components::EntityComponents Service::iterable::iterator::operator*() {
    return Components::EntityComponents(mIt.id());
}

Service::iterable::iterator& Service::iterable::iterator::operator++() {
    do {
        ++mIt;
    } while (mIt != mEnd && !mCheck(mIt.id()));
    return *this;
}

Service::iterable::iterator::operator bool() const {
    return mIt != mEnd && mCheck(mIt.id());
}

// Service::iterable
Service::iterable::iterable(const iterator_base& bIt, const iterator_base& eIt,
                            const CheckFunc& check)
    : mBegin(bIt, eIt, check), mEnd(eIt, eIt, check) {}

Service::iterable::iterator Service::iterable::begin() { return mBegin; }
Service::iterable::iterator Service::iterable::end() { return mEnd; }
}  // namespace Services