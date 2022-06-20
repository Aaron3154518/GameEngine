#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <type_traits>

struct NoData
{
};

template <class Data, class RetT, class... ArgTs>
class Subscription
{
public:
    typedef std::function<RetT(ArgTs...)> Function;

    Subscription()
        : subscription([](ArgTs... args) {}), status(std::make_shared<bool>(false)) {}
    Subscription(Function func)
        : subscription(func), status(std::make_shared<bool>(true)) {}
    ~Subscription()
    {
        // unsubscribe();
    }

    void unsubscribe()
    {
        *status = false;
    }

    operator bool() const
    {
        return *status;
    }

    RetT operator()(ArgTs... args) const
    {
        return subscription(args...);
    }

    Data data;

private:
    Function subscription;

    std::shared_ptr<bool> status;
};

template <class T, class RetT, class Data = NoData, class... ArgTs>
class Observable
{
    Observable() { std::cerr << "Full Template" << std::endl; }
    ~Observable() = default;
};

template <class T, class RetT, class Data, class... ArgTs>
class Observable<T, RetT(ArgTs...), Data>
{
public:
    typedef Subscription<std::shared_ptr<Data>, RetT, ArgTs...> Subscription;
    typedef std::list<Subscription> SubscriptionList;
    typedef typename SubscriptionList::iterator SubscriptionIterator;

    Observable() { std::cerr << "Partial Specializtion" << std::endl; }
    ~Observable() = default;

    Subscription subscribe(Subscription::Function func, std::shared_ptr<Data> data = nullptr)
    {
        Subscription sub(func);
        sub.data = data;
        mSubscriptions.push_back(sub);
        std::cerr << "Subscribe" << std::endl;
        return sub;
    }

    virtual void next(T val)
    {
        std::cerr << "Called Next" << std::endl;
        if constexpr (std::is_same<typename Subscription::Function,
                                   typename ::Subscription<Data, RetT, T>::Function>::value)
        {
            std::cerr << "Default" << std::endl;
            forEachSubscription([&](Subscription &s) -> bool
                                { s(val); return true; });
        }
    }

protected:
    void forEachSubscription(const std::function<bool(Subscription &)> &func)
    {
        std::cerr << "For Each" << std::endl;
        for (auto it = mSubscriptions.begin(); it != mSubscriptions.end(); it = nextSubscription(it))
        {
            if (!func(*it))
            {
                break;
            }
        }
        std::cerr << "End Foreach" << std::endl;
    }

    SubscriptionIterator &nextSubscription(SubscriptionIterator &current)
    {
        if (current == mSubscriptions.end())
        {
            return current;
        }

        ++current;
        if (current != mSubscriptions.end() && !*current)
        {
            current = mSubscriptions.erase(current);
        }
        return current;
    }

    SubscriptionList mSubscriptions;
};

/*
template <class T, class RetT, class... ArgTs>
class Observable<T, Subscription<RetT, ArgTs...>, void>
{
public:
    typedef Subscription<RetT, ArgTs...> Subscription;
    typedef std::list<Subscription> SubscriptionList;
    typedef typename SubscriptionList::iterator SubscriptionIterator;

    Subscription subscribe(Subscription::Function func)
    {
        Subscription sub(func);
        mSubscriptions.push_back(sub);
        return sub;
    }

    virtual void next(T val)
    {
        std::cerr << mSubscriptions.size() << std::endl;
        if constexpr (std::is_same<Subscription, ::Subscription<RetT, T>>::value)
        {
            for (auto it = mSubscriptions.begin(); it != mSubscriptions.end(); it = nextSubscription(it))
            {
                std::cerr << (it == mSubscriptions.end()) << std::endl;
                (*it)(val);
            }
        }
    }

protected:
    SubscriptionIterator &nextSubscription(SubscriptionIterator &current)
    {
        if (current == mSubscriptions.end())
        {
            return current;
        }

        ++current;
        if (!*current)
        {
            current = mSubscriptions.erase(current);
        }
        return current;
    }

    SubscriptionList mSubscriptions;
};*/

#endif
