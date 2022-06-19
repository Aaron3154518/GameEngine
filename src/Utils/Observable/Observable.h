#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <list>

struct Unsubscribe
{
    void unsubscribe()
    {
        mUnsubscribe();
    }

private:
    void (*mUnsubscribe)();
};

template <class T>
class BaseObservable
{
public:
    typedef void (*Subscription)(T);

    BaseObservable() = default;
    ~BaseObservable() = default;
};

template <class T>
class Observable : public BaseObservable
{
public:
    Unsubscribe subscribe(Subscription sub)
    {
        auto &it = mSubscriptions.insert(mSubscriptions.begin(), sub);
        return Unsubscribe{[this, &it]()
                           {
                               mSubscriptions.erase(it);
                           }};
    }

    void next(T val)
    {
        for (Subscription &sub : mSubscriptions)
        {
            sub(val);
        }
    }

private:
    std::list<Subscription> mSubscriptions;
};

#endif
