#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <Utils/Range.h>

#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <memory>

// Helper structs/typedefs
template <class... Ts>
struct Wrapper {};

template <class T>
using Func = std::function<T>;

template <class T>
struct SubscriptionType {
    SubscriptionType(T t) : mVal(t) {}

    T mVal;
};

template <std::size_t i, class T>
struct SubImpl : SubscriptionType<T> {
    using SubscriptionType<T>::SubscriptionType;
};

// Observable
// For storing different observables in a container
struct ObservableBase {};

// This struct is used to combine all arguments into one parameter pack
// Apparently g++ doesn't like multiple variadic inheritance
template <class, class>
struct ObservableImplBase;

template <class... ArgTs, class... BaseTs>
struct ObservableImplBase<Wrapper<ArgTs...>, Wrapper<BaseTs...>>
    : public ObservableBase {
   public:
    struct SubscriptionT : public BaseTs... {
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
        Iterator(const ItT& it, const ItT& end) : mIt(it), mEnd(end) {
            while (mIt != mEnd && !mIt->lock()) {
                std::cerr << "Sup" << std::endl;
                ++mIt;
            }
        }

        // Prefix ++
        Iterator& operator++() {
            ++mIt;
            while (mIt != mEnd && !mIt->lock()) {
                ++mIt;
            }
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

        SubscriptionPtr operator*() { return mIt->lock(); }

        operator bool() const { return mIt != mEnd && mIt->lock(); }

       private:
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

    auto begin() {
        return _begin(mSubscriptions.begin(), mSubscriptions.end());
    }
    auto rbegin() {
        return _begin(mSubscriptions.rbegin(), mSubscriptions.rend());
    }
    auto begin() const {
        return _begin(mSubscriptions.cbegin(), mSubscriptions.cend());
    }
    auto rbegin() const {
        return _begin(mSubscriptions.crbegin(), mSubscriptions.crend());
    }
    auto end() { return _end(mSubscriptions.end()); }
    auto rend() { return _end(mSubscriptions.rend()); }
    auto end() const { return _end(mSubscriptions.cend()); }
    auto rend() const { return _end(mSubscriptions.crend()); }

   private:
    template <class ItT>
    Iterator<ItT> _begin(const ItT& begin, const ItT& end) {
        prune();
        return Iterator<ItT>(begin, end);
    }

    template <class ItT>
    Iterator<ItT> _end(const ItT& end) {
        return Iterator<ItT>(end, end);
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

// For convenience
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

template <class... Ts>
using ForwardObservable = ForwardObservableImpl<0, Wrapper<>, Wrapper<>, Ts...>;

#endif