#ifndef LOCKABLE_H
#define LOCKABLE_H

#include <algorithm>
#include <list>
#include <memory>

struct Lock {
    friend class Lockable;
    friend class std::hash<Lock>;

   public:
    Lock();
    Lock(void* lock);

    operator bool() const;

    bool operator==(void* rhs) const;
    bool operator!=(void* rhs) const;

   private:
    void* mLock;
};

namespace std {
template <>
struct hash<Lock> {
    std::size_t operator()(const Lock& l) const {
        return hash<void*>()(l.mLock);
    }
};
}  // namespace std

class Lockable {
   public:
    virtual ~Lockable() = default;

    virtual Lock requestLock();
    virtual void releaseLock(Lock& lock);
    virtual void releaseLock(void* lock);
    virtual bool isLocked() const;

    const std::list<std::unique_ptr<bool>>& locks() const;

   private:
    std::list<std::unique_ptr<bool>> mLocks;
};

#endif
