#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <functional>
#include <list>

struct Unsubscribe
{
    std::function<void()> unsubscribe;
};

template <class T>
class BaseObservable
{
public:
    typedef std::function<void(const T &)> Subscription;

    BaseObservable() = default;
    ~BaseObservable() = default;
};

template <class T>
class Observable : public BaseObservable<T>
{
public:
    typedef typename BaseObservable<T>::Subscription Subscription;

    Unsubscribe subscribe(Subscription sub)
    {
        auto it = mSubscriptions.insert(mSubscriptions.begin(), sub);
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
