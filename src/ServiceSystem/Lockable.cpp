#include "Lockable.h"

// Lock
Lock::Lock() : Lock(nullptr) {}
Lock::Lock(bool* lock) : mLock(lock) {}

Lock::operator bool() const { return mLock != nullptr; }

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
