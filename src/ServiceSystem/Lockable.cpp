#include "Lockable.h"

// Lock
Lock::Lock() : Lock(nullptr) {}
Lock::Lock(void* lock) : mLock(lock) {}

Lock::operator bool() const { return mLock; }

bool Lock::operator==(void* rhs) const { return mLock == rhs; }
bool Lock::operator!=(void* rhs) const { return !(*this == rhs); }

// Lockable
Lock Lockable::requestLock() {
    return Lock(mLocks.insert(mLocks.end(), std::make_unique<bool>())->get());
}

void Lockable::releaseLock(Lock& lock) {
    auto it = std::find_if(mLocks.begin(), mLocks.end(),
                           [lock](const std::unique_ptr<bool>& ptr) -> bool {
                               return ptr.get() == lock.mLock;
                           });
    if (it != mLocks.end()) {
        mLocks.erase(it);
    }
    lock.mLock = nullptr;
}

bool Lockable::isLocked() const { return mLocks.size() > 0; }

const std::list<std::unique_ptr<bool>>& Lockable::locks() const {
    return mLocks;
}
