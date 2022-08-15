#ifndef LOCKABLE_H
#define LOCKABLE_H

#include <algorithm>
#include <list>
#include <memory>

struct Lock {
    friend class Lockable;

    Lock();
    Lock(bool* lock);

    operator bool() const;

   private:
    bool* mLock;
};

class Lockable {
   public:
    Lock requestLock();
    void releaseLock(Lock& lock);
    bool isLocked() const;

   private:
    std::list<std::unique_ptr<bool>> mLocks;
};

#endif
