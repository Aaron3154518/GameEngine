#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <Utils/Range.h>

#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <memory>

// Helper structs/typedefs
template <class... Ts>
struct Wrapper {};

template <class T>
using Func = std::function<T>;

struct SubscriptionBase {
   public:
    void setActive(bool active) { mActive = active; }
    bool isActive() const { return mActive; }

   private:
    bool mActive = true;
};

template <class T>
struct SubscriptionType {
    SubscriptionType(T t) : mVal(t) {}

    T mVal;
};

template <class T>
using SubscriptionTypePtr = std::shared_ptr<SubscriptionType<T>>;
template <class T>
using SubscriptionTypeWPtr = std::weak_ptr<SubscriptionType<T>>;

template <std::size_t i, class T>
struct SubImpl : SubscriptionType<T> {
    using SubscriptionType<T>::SubscriptionType;
};

// Observable
// For storing different observables in a container
struct ObservableBase {
    virtual ~ObservableBase() = default;
};

// This struct is used to combine all arguments into one parameter pack
// Apparently g++ doesn't like multiple variadic inheritance
template <class, class>
struct ObservableImplBase;

template <class... ArgTs, class... BaseTs>
struct ObservableImplBase<Wrapper<ArgTs...>, Wrapper<BaseTs...>>
    : public ObservableBase {
   public:
    struct SubscriptionT : public SubscriptionBase, public BaseTs... {
       public:
        SubscriptionT(ArgTs... args) : BaseTs(args)... {}

        template <int N>
        using ArgType =
            typename std::tuple_element<N, std::tuple<ArgTs...>>::type;

        template <int N>
        using BaseType =
            typename std::tuple_element<N, std::tuple<BaseTs...>>::type;

        template <size_t I>
        std::conditional_t<std::is_function<ArgType<I>>::value, ArgType<I>,
                           ArgType<I>&>
        get() {
            return static_cast<BaseType<I>*>(this)->mVal;
        }
    };

    typedef std::shared_ptr<SubscriptionT> SubscriptionPtr;
    typedef std::weak_ptr<SubscriptionT> SubscriptionWPtr;

    template <class ItT>
    struct Iterator {
        friend class ObservableImplBase<Wrapper<ArgTs...>, Wrapper<BaseTs...>>;

       public:
        Iterator(const ItT& end) : Iterator(end, end) {}
        Iterator(const ItT& it, const ItT& end) : mIt(it), mEnd(end) {
            skipInvalid();
        }

        // Prefix ++
        Iterator& operator++() {
            ++mIt;
            skipInvalid();
            return *this;
        }
        // Postfix ++
        Iterator operator++(int) {
            Iterator old(mIt, mEnd);
            ++*this;
            return old;
        }

        bool operator==(const Iterator& it) { return mIt == it.mIt; }
        bool operator!=(const Iterator& it) { return mIt != it.mIt; }

        SubscriptionPtr operator*() {
            if (!mIt->lock()) {
                throw std::runtime_error(
                    "Attempted to dereference iterator to null subscription: " +
                    std::string(typeid(this).name()));
            }
            return mIt->lock();
        }

        operator bool() const { return mIt != mEnd && mIt->lock(); }

       private:
        bool isInvalid() const {
            if (mIt == mEnd) {
                return false;
            }

            auto sub = mIt->lock();
            return !sub || !sub->isActive();
        }

        void skipInvalid() {
            while (isInvalid()) {
                ++mIt;
            }
        }

        ItT mIt, mEnd;
    };
    typedef std::list<SubscriptionWPtr> SubscriptionList;
    typedef Iterator<typename SubscriptionList::iterator> iterator;
    typedef Iterator<typename SubscriptionList::reverse_iterator>
        reverse_iterator;
    typedef Iterator<typename SubscriptionList::const_iterator> const_iterator;
    typedef Iterator<typename SubscriptionList::const_reverse_iterator>
        const_reverse_iterator;

    SubscriptionPtr subscribe(ArgTs... args) {
        SubscriptionPtr sub = std::make_shared<SubscriptionT>(args...);
        mSubscriptions.push_back(sub);
        onSubscribe(sub);
        return sub;
    };

    void subscribe(const SubscriptionPtr& sub) {
        mSubscriptions.push_back(sub);
    };

   protected:
    SubscriptionList mSubscriptions;

    virtual void onSubscribe(SubscriptionPtr sub) {}

    // Prune unsubscribed
    void prune() {
        for (auto it = mSubscriptions.begin(), end = mSubscriptions.end();
             it != end; ++it) {
            while (it != end && !it->lock()) {
                it = mSubscriptions.erase(it);
            }
            if (it == end) {
                break;
            }
        }
    }

    iterator erase(const iterator& it) {
        return iterator(mSubscriptions.erase(it.mIt), it.mEnd);
    }

    iterator begin() {
        prune();
        return iterator(mSubscriptions.begin(), mSubscriptions.end());
    }
    reverse_iterator rbegin() {
        prune();
        return reverse_iterator(mSubscriptions.rbegin(), mSubscriptions.rend());
    }
    const_iterator begin() const {
        prune();
        return const_iterator(mSubscriptions.cbegin(), mSubscriptions.cend());
    }
    const_reverse_iterator rbegin() const {
        prune();
        return const_reverse_iterator(mSubscriptions.crbegin(),
                                      mSubscriptions.crend());
    }
    iterator end() { return iterator(mSubscriptions.end()); }
    reverse_iterator rend() { return reverse_iterator(mSubscriptions.rend()); }
    const_iterator end() const { return const_iterator(mSubscriptions.cend()); }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(mSubscriptions.crend());
    }
};

// This struct is used to separate and enumerate data and function classes
template <size_t, class, class, class...>
struct ObservableImpl;

template <size_t Ctr, size_t... Idxs, class... Ts, class DataT, class... Tail>
struct ObservableImpl<Ctr, Range<Idxs...>, Wrapper<Ts...>, DataT, Tail...>
    : public ObservableImpl<Ctr + 1, Range<Idxs..., Ctr>, Wrapper<Ts..., DataT>,
                            Tail...> {};

template <size_t Ctr, size_t... Idxs, class... Ts, class RetT, class... ArgTs,
          class... Tail>
struct ObservableImpl<Ctr, Range<Idxs...>, Wrapper<Ts...>, RetT(ArgTs...),
                      Tail...>
    : public ObservableImpl<Ctr + 1, Range<Idxs..., Ctr>,
                            Wrapper<Ts..., Func<RetT(ArgTs...)>>, Tail...> {};

template <size_t Ctr, size_t... Idxs, class... Ts>
struct ObservableImpl<Ctr, Range<Idxs...>, Wrapper<Ts...>>
    : public ObservableImplBase<Wrapper<Ts...>, Wrapper<SubImpl<Idxs, Ts>...>> {
};

// Expose the implementation
template <class... Ts>
using Observable = ObservableImpl<0, Range<>, Wrapper<>, Ts...>;

// ForwardObservable
template <size_t, class, class, class...>
struct ForwardObservableImpl;

template <size_t i, class... FuncTs, class... SubTs, class... ArgTs>
struct ForwardObservableImpl<i, Wrapper<FuncTs...>, Wrapper<SubTs...>,
                             void(ArgTs...)>
    : public ObservableImplBase<
          Wrapper<FuncTs..., Func<void(ArgTs...)>>,
          Wrapper<SubTs..., SubImpl<i, Func<void(ArgTs...)>>>> {
   public:
    virtual void next(ArgTs... t) {
        for (auto sub : *this) {
            sub->template get<i>()(t...);
        }
    }

   protected:
    using ObservableImplBase<
        Wrapper<FuncTs..., Func<void(ArgTs...)>>,
        Wrapper<SubTs..., SubImpl<i, Func<void(ArgTs...)>>>>::mSubscriptions;
};

template <size_t i, class... FuncTs, class... SubTs, class... ArgTs,
          class... Tail>
struct ForwardObservableImpl<i, Wrapper<FuncTs...>, Wrapper<SubTs...>,
                             void(ArgTs...), Tail...>
    : public ForwardObservableImpl<
          i + 1, Wrapper<FuncTs..., Func<void(ArgTs...)>>,
          Wrapper<SubTs..., SubImpl<i, Func<void(ArgTs...)>>>, Tail...> {
    typedef ForwardObservableImpl<
        i + 1, Wrapper<FuncTs..., Func<void(ArgTs...)>>,
        Wrapper<SubTs..., SubImpl<i, Func<void(ArgTs...)>>>, Tail...>
        Base;

   public:
    using Base::next;

    virtual void next(ArgTs... t) {
        for (auto sub : *this) {
            sub->template get<i>()(t...);
        }
    }

   protected:
    using Base::mSubscriptions;
};

// Expose the implementation
template <class... Ts>
using ForwardObservable = ForwardObservableImpl<0, Wrapper<>, Wrapper<>, Ts...>;

// ReplyObservable
template <class>
struct ReplyObservable;

template <class ResT, class... ReqTs>
struct ReplyObservable<ResT(ReqTs...)> {
   public:
    class RequestObservable
        : public ObservableImplBase<
              Wrapper<std::function<ResT(ReqTs...)>>,
              Wrapper<SubImpl<0, std::function<ResT(ReqTs...)>>>> {
        friend class ReplyObservable<ResT(ReqTs...)>;
    };

    class ResponseObservable
        : public ObservableImplBase<
              Wrapper<std::function<void(ResT)>>,
              Wrapper<SubImpl<0, std::function<void(ResT)>>>> {
        friend class ReplyObservable<ResT(ReqTs...)>;
    };

    ReplyObservable() = default;
    virtual ~ReplyObservable() = default;

    virtual void next(ReqTs... args) {
        for (auto reqSub : mRequestObservable) {
            ResT message = reqSub->template get<0>()(args...);
            for (auto resSub : mResponseObservable) {
                resSub->template get<0>()(message);
            }
        }
    }

    typename RequestObservable::SubscriptionPtr subscribeToRequest(
        std::function<ResT(ReqTs...)> func) {
        return mRequestObservable.subscribe(func);
    }

    typename ResponseObservable::SubscriptionPtr subscribeToResponse(
        std::function<void(ResT)> func) {
        return mResponseObservable.subscribe(func);
    }

   protected:
    RequestObservable mRequestObservable;
    ResponseObservable mResponseObservable;
};

// Specialization for ResT = void
template <class... ReqTs>
struct ReplyObservable<void(ReqTs...)> {
   public:
    class RequestObservable
        : public ObservableImplBase<
              Wrapper<std::function<void(ReqTs...)>>,
              Wrapper<SubImpl<0, std::function<void(ReqTs...)>>>> {
        friend class ReplyObservable<void(ReqTs...)>;
    };

    class ResponseObservable : public ObservableImplBase<
                                   Wrapper<std::function<void()>>,
                                   Wrapper<SubImpl<0, std::function<void()>>>> {
        friend class ReplyObservable<void(ReqTs...)>;
    };

    ReplyObservable() = default;
    virtual ~ReplyObservable() = default;

    virtual void next(ReqTs... args) {
        for (auto reqSub : mRequestObservable) {
            reqSub->template get<0>()(args...);
            for (auto resSub : mResponseObservable) {
                resSub->template get<0>()();
            }
        }
    }

    typename RequestObservable::SubscriptionPtr subscribeToRequest(
        std::function<void(ReqTs...)> func) {
        return mRequestObservable.subscribe(func);
    }

    typename ResponseObservable::SubscriptionPtr subscribeToResponse(
        std::function<void()> func) {
        return mResponseObservable.subscribe(func);
    }

   protected:
    RequestObservable mRequestObservable;
    ResponseObservable mResponseObservable;
};

#endif