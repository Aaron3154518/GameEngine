#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <type_traits>

struct Unsubscriber {
   public:
    void unsubscribe();

    operator bool() const;

   private:
    std::shared_ptr<bool> mSubscribed = std::make_shared<bool>(true);
};

template <class Data, class RetT, class... ArgTs>
class Subscription {
   public:
    typedef std::function<RetT(ArgTs...)> Function;

    Subscription(Function func)
        : subscription(func) {}
    ~Subscription() = default;

    void setUnsubscriber(Unsubscriber unsub) {
        unsubscriber = unsub;
    }

    Unsubscriber getUnsubscriber() const {
        return unsubscriber;
    }

    // Unsubscribes only this subscription
    void unsubscribe() {
        unsubscriber = Unsubscriber();
        unsubscriber.unsubscribe();
    }

    void changeSubscription(Function func) {
        subscription = func;
    }

    operator bool() const {
        return unsubscriber;
    }

    RetT operator()(ArgTs... args) const {
        return subscription(args...);
    }

    // Only include data if not void
    std::enable_if_t<!std::is_same<Data, void>::value, Data> data;

   private:
    Function subscription;

    Unsubscriber unsubscriber;
};

template <class T, class... Args>
struct is_simple : std::false_type {};

template <class T>
struct is_simple<T, T> : std::true_type {};

// Full template, not usable
template <class T, class RetT, class Data = void, class... ArgTs>
class Observable {
    static_assert(!std::is_same<T, T>::value, "Must use specialized observable");
};

// Partially specialized template, use this
template <class T, class RetT, class Data, class... ArgTs>
class Observable<T, RetT(ArgTs...), Data> {
   public:
    typedef Subscription<std::shared_ptr<Data>, RetT, ArgTs...> SubscriptionT;
    typedef std::shared_ptr<SubscriptionT> SubscriptionPtr;
    typedef std::list<SubscriptionPtr> SubscriptionList;
    typedef typename SubscriptionList::iterator SubscriptionIterator;

    Observable() = default;
    ~Observable() = default;

    // Used when no subscriber data

    // Generates standard subscription
    template <int N = 0>
    typename std::enable_if_t<std::is_same<Data, void>::value && N == N, SubscriptionPtr>
    subscribe(typename SubscriptionT::Function func) {
        SubscriptionPtr sub = std::make_shared<SubscriptionT>(func);
        mSubscriptions.push_back(sub);
        return sub;
    }

    // Takes existing subscription, creates new if null else updates existing
    template <int N = 0>
    typename std::enable_if_t<std::is_same<Data, void>::value && N == N, SubscriptionPtr>
    updateSubscription(SubscriptionPtr &sub, typename SubscriptionT::Function func) {
        auto it = std::find(mSubscriptions.begin(), mSubscriptions.end(), sub);
        if (it == mSubscriptions.end()) {
            sub = subscribe(func);
        } else {
            sub->changeSubscription(func);
        }
        return sub;
    }

    // Used when subscriber has data
    template <int N = 0>
    typename std::enable_if_t<!std::is_same<Data, void>::value && N == N, SubscriptionPtr>
    subscribe(typename SubscriptionT::Function func, std::shared_ptr<Data> data) {
        SubscriptionPtr sub = std::make_shared<SubscriptionT>(func);
        sub->data = data;
        mSubscriptions.push_back(sub);
        return sub;
    }

    template <int N = 0>
    typename std::enable_if_t<!std::is_same<Data, void>::value && N == N, SubscriptionPtr>
    updateSubscription(SubscriptionPtr &sub, typename SubscriptionT::Function func, std::shared_ptr<Data> data) {
        auto it = std::find(mSubscriptions.begin(), mSubscriptions.end(), sub);
        if (it == mSubscriptions.end()) {
            sub = subscribe(func, data);
        } else {
            sub->changeSubscription(func);
            sub->data = data;
        }
        return sub;
    }

    virtual void next(T val) {
        removeUnsubscribed();
        serve(val);
    }

   protected:
    virtual void serve(T val) {
        defaultServe(val);
    }

    virtual bool unsubscribe(SubscriptionPtr sub) {
        return true;
    }

    void removeUnsubscribed() {
        for (auto it = mSubscriptions.begin(); it != mSubscriptions.end(); ++it) {
            if (!**it && unsubscribe(*it)) {
                it = mSubscriptions.erase(it);
                if (it == mSubscriptions.end()) {
                    break;
                }
            }
        }
    }

    SubscriptionIterator &nextSubscription(SubscriptionIterator &current) {
        if (current == mSubscriptions.end()) {
            return current;
        }

        ++current;
        if (current != mSubscriptions.end() && !**current) {
            current = mSubscriptions.erase(current);
        }
        return current;
    }

    SubscriptionList mSubscriptions;

   private:
    // Case 1: ArgTs = T => defaultServe() will send subscribers data.
    template <int N = 0>
    typename std::enable_if_t<is_simple<T, ArgTs...>::value && N == N>
    defaultServe(T val) {
        for (auto sub : mSubscriptions) {
            (*sub)(val);
        }
    }

    // Case 2: ArgTs != T => serve functionality is left to the inheritor
    template <int N = 0>
    typename std::enable_if_t<!is_simple<T, ArgTs...>::value && N == N>
    defaultServe(T val) {}
};

#endif
