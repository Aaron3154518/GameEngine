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

template <class T, class... Args>
struct is_simple : std::false_type {};

template <class T>
struct is_simple<T, T> : std::true_type {};

class ObservableBase {};

// Full template, not usable
template <class T, class RetT, class DaDataTta = void, class... ArgTs>
class Observable : public ObservableBase {
    static_assert(!std::is_same<T, T>::value, "Must use specialized observable");
};

// Partially specialized template, use this
template <class T, class RetT, class DataT, class... ArgTs>
class Observable<T, RetT(ArgTs...), DataT> : public ObservableBase {
   private:
    template <class Data, class Enable = void>
    class SubscriptionBase {};

    template <class Data>
    class SubscriptionBase<Data, std::enable_if_t<!std::is_same<Data, void>::value>> {
       public:
        const std::shared_ptr<Data> &getData() const {
            return data;
        }

       protected:
        std::shared_ptr<Data> data;
    };

   public:
    class Subscription : public SubscriptionBase<DataT> {
        friend class Observable<T, RetT(ArgTs...), DataT>;

       public:
        typedef std::function<RetT(ArgTs...)> Function;

        Subscription(Function func) : subscription(func) {}
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

        operator bool() const {
            return unsubscriber;
        }

        RetT operator()(ArgTs... args) const {
            return subscription(args...);
        }

       private:
        Function subscription;

        Unsubscriber unsubscriber;
    };
    typedef std::shared_ptr<Subscription> SubscriptionPtr;
    typedef std::list<SubscriptionPtr> SubscriptionList;
    typedef typename SubscriptionList::iterator SubscriptionIterator;

    Observable() = default;
    ~Observable() = default;

    // Used when no subscriber data
    // Generates standard subscription
    template <int N = 0>
    typename std::enable_if_t<std::is_same<DataT, void>::value && N == N, SubscriptionPtr>
    subscribe(typename Subscription::Function func) {
        SubscriptionPtr sub = std::make_shared<Subscription>(func);
        mSubscriptions.push_back(sub);
        return sub;
    }

    // Used when subscriber has data
    template <int N = 0>
    typename std::enable_if_t<!std::is_same<DataT, void>::value && N == N, SubscriptionPtr>
    subscribe(typename Subscription::Function func, std::shared_ptr<DataT> data) {
        SubscriptionPtr sub = std::make_shared<Subscription>(func);
        sub->data = data;
        mSubscriptions.push_back(sub);
        return sub;
    }

    void updateSubscription(SubscriptionPtr &sub, typename Subscription::Function func) {
        sub->subscription = func;
    }

    template <int N = 0>
    typename std::enable_if_t<!std::is_same<DataT, void>::value && N == N>
    updateSubscriptionData(SubscriptionPtr &sub, std::shared_ptr<DataT> data) {
        sub->data = data;
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
