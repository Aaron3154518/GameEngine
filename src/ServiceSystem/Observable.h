#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
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
template <class T, class RetT, class DataT = void, class... ArgTs>
class Observable : public ObservableBase {
    static_assert(!std::is_same<T, T>::value,
                  "Must use specialized observable");
};

// Partially specialized template, use this
template <class T, class RetT, class DataT, class... ArgTs>
class Observable<T, RetT(ArgTs...), DataT> : public ObservableBase {
   private:
    template <class Data, class Enable = void>
    class SubscriptionBase {};

    template <class Data>
    class SubscriptionBase<Data,
                           std::enable_if_t<!std::is_same<Data, void>::value>> {
       public:
        const std::shared_ptr<Data>& getData() const { return data; }

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

        void setUnsubscriber(Unsubscriber unsub) { unsubscriber = unsub; }

        Unsubscriber getUnsubscriber() const { return unsubscriber; }

        // Unsubscribes only this subscription
        void unsubscribe() {
            unsubscriber = Unsubscriber();
            unsubscriber.unsubscribe();
        }

        operator bool() const { return unsubscriber; }

       private:
        RetT operator()(ArgTs... args) const { return subscription(args...); }

        Function subscription;

        Unsubscriber unsubscriber;
    };
    typedef std::shared_ptr<Subscription> SubscriptionPtr;
    typedef std::list<SubscriptionPtr> SubscriptionList;
    /*
        class SubscriptionList {
           public:
            SubscriptionList() = default;
            ~SubscriptionList() = default;

            struct Iterator {
                using iterator_category = std::forward_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = Subscription;
                using pointer = std::list<SubscriptionPtr>::iterator;
                using reference = SubscriptionPtr;

                Iterator(pointer end, std::function(pointer(pointer)) erase)
                    : mPtr(end), mEnd(end), mErase(erase) {}
                Iterator(pointer ptr, pointer end,
                         std::function(pointer(pointer)) erase)
                    : mPtr(ptr), mEnd(end), mErase(erase) {
                    while (mPtr != mEnd && !**mPtr) {
                        mPtr = mErase(mPtr);
                    }
                }

                reference operator*() const { return *mPtr; }
                pointer operator->() { return mPtr; }
                Iterator& operator++() {
                    mPtr++;
                    while (mPtr != mEnd && !**mPtr) {
                        mPtr = mErase(mPtr);
                    }
                    return *this;
                }
                Iterator operator++(int) {
                    Iterator tmp = *this;
                    ++(*this);
                    return tmp;
                }
                friend bool operator==(const Iterator& a, const Iterator& b) {
                    return a.mPtr == b.mPtr;
                };
                friend bool operator!=(const Iterator& a, const Iterator& b) {
                    return a.mPtr != b.mPtr;
                };

               private:
                pointer mPtr, mEnd;
                std::function(iterator(iterator)) mErase;
            };

            Iterator begin() {
                return Iterator(mSubscriptions.begin(), mSubscriptions.end(),
                                [this](std::list<SubscriptionPtr>::iterator it)
       { return mSubscriptions.erase(it);
                                });
            }
            Iterator end() {
                return Iterator(mSubscriptions.end(),
                                [this](std::list<SubscriptionPtr>::iterator it)
       { return mSubscriptions.erase(it);
                                });
            }

            std::list<SubscriptionPtr> mSubscriptions;
        };*/

    Observable() = default;
    ~Observable() = default;

    // Used when no subscriber data
    // Generates standard subscription
    template <int N = 0>
    std::enable_if_t<std::is_same<DataT, void>::value && N == N,
                     SubscriptionPtr>
    subscribe(typename Subscription::Function func) {
        SubscriptionPtr sub = std::make_shared<Subscription>(func);
        mSubscriptions.push_back(sub);
        return sub;
    }

    // Used when subscriber has data
    template <int N = 0>
    std::enable_if_t<!std::is_same<DataT, void>::value && N == N,
                     SubscriptionPtr>
    subscribe(typename Subscription::Function func,
              std::shared_ptr<DataT> data) {
        SubscriptionPtr sub = std::make_shared<Subscription>(func);
        sub->data = data;
        mSubscriptions.push_back(sub);
        return sub;
    }

    void updateSubscription(SubscriptionPtr& sub,
                            typename Subscription::Function func) {
        sub->subscription = func;
    }

    template <int N = 0>
    std::enable_if_t<!std::is_same<DataT, void>::value && N == N>
    updateSubscriptionData(SubscriptionPtr& sub, std::shared_ptr<DataT> data) {
        sub->data = data;
    }

    virtual void next(T val) {
        prune();
        serve(val);
    }

   protected:
    virtual void onUnsubscribe(SubscriptionPtr sub) {}

    virtual void prune() {
        for (auto it = mSubscriptions.begin(), end = mSubscriptions.end();
             it != end; ++it) {
            if (!**it) {
                onUnsubscribe(*it);
                it = mSubscriptions.erase(it);
                if (it == end) {
                    break;
                }
            }
        }
    }

    virtual void serve(T val) { defaultServe(val); }

    // No return value
    template <int N = 0>
    std::enable_if_t<std::is_same<RetT, void>::value && N == N, bool> call(
        typename SubscriptionList::iterator it, ArgTs... args) {
        return call(*it, args...);
    }
    template <int N = 0>
    std::enable_if_t<std::is_same<RetT, void>::value && N == N, bool> call(
        SubscriptionPtr sub, ArgTs... args) {
        if (*sub) {
            (*sub)(args...);
            return true;
        }
        return false;
    }

    // Yes return value
    template <int N = 0>
    std::enable_if_t<!std::is_same<RetT, void>::value && N == N, bool> call(
        typename SubscriptionList::iterator it, RetT* retVal, ArgTs... args) {
        return call(*it, retVal, args...);
    }
    template <int N = 0>
    std::enable_if_t<!std::is_same<RetT, void>::value && N == N, bool> call(
        SubscriptionPtr sub, RetT* retVal, ArgTs... args) {
        if (*sub) {
            *retVal = (*sub)(args...);
            return true;
        }
        return false;
    }

    /*
        SubscriptionIterator& begin() {
            auto it = mSubscriptions.begin();
            while (it != end() && !**it) {
                it = mSubscriptions.erase(it);
            }
            return it;
        }
        SubscriptionIterator& end() { return mSubscriptions.end(); }
        SubscriptionIterator& next(SubscriptionIterator& curr) {
            if (curr != end()) {
                ++curr;
            }
            while (curr != end() && !**curr) {
                curr = mSubscriptions.erase(curr);
            }
            return curr;
        }
    */
    SubscriptionList mSubscriptions;

   private:
    // Case 1: ArgTs = T => defaultServe() will send subscribers data.
    template <int N = 0>
    std::enable_if_t<is_simple<T, ArgTs...>::value && N == N> defaultServe(
        T val) {
        for (auto sub : mSubscriptions) {
            call(sub, val);
        }
    }

    // Case 2: ArgTs != T => serve functionality is left to the inheritor
    template <int N = 0>
    std::enable_if_t<!is_simple<T, ArgTs...>::value && N == N> defaultServe(
        T val) {}
};

#endif
