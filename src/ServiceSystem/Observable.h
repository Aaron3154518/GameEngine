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

// Subscription Functions
// Use SubFuncBase to match all subscriptions with the given function signature
template <class>
struct SubFuncBase;

template <class RetT, class... ArgTs>
struct SubFuncBase<Func<RetT(ArgTs...)>> {
   public:
    typedef Func<RetT(ArgTs...)> Function;

    SubFuncBase(Function func) : mFunc(func) {}

   protected:
    Function mFunc;
};

template <size_t, class>
struct SubFuncImpl;

template <size_t i, class RetT, class... ArgTs>
struct SubFuncImpl<i, Func<RetT(ArgTs...)>>
    : public SubFuncBase<Func<RetT(ArgTs...)>> {
    template <std::size_t _i, class _RetT, class... _ArgTs>
    friend _RetT call(SubFuncImpl<_i, Func<_RetT(_ArgTs...)>>&, _ArgTs...);
};

template <std::size_t i, class RetT, class... ArgTs>
RetT call(SubFuncImpl<i, Func<RetT(ArgTs...)>>& t, ArgTs... args) {
    return t.mFunc(args...);
}

// Subscription Data
// Use SubDataBase to match all subscriptions with the given data type
template <class DataT>
struct SubDataBase {
   public:
    SubDataBase(const DataT& data) : mData(data) {}

   protected:
    DataT mData;
};

template <size_t i, class DataT>
struct SubDataImpl : public SubDataBase<DataT> {
    template <std::size_t _i, class _DataT>
    friend _DataT& get(SubDataImpl<_i, _DataT>&);

    template <std::size_t _i, class _DataT>
    friend void set(SubDataImpl<_i, _DataT>&, const _DataT&);
};

template <std::size_t i = 0, class DataT>
DataT& get(SubDataImpl<i, DataT>& t) {
    return t.mData;
}

template <std::size_t i, class DataT>
void set(SubDataImpl<i, DataT>& t, const DataT& data) {
    t.mData = std::forward<DataT>(data);
}

// Observable
// For storing different observables in a container
struct ObservableBase {};

// This struct is used to combine all arguments into one parameter pack
// Apparently g++ doesn't like multiple variadic inheritance
template <class, class>
struct ObservableImplBase;

template <class... ArgTs, class... InherTs>
struct ObservableImplBase<Wrapper<ArgTs...>, Wrapper<InherTs...>>
    : public ObservableBase {
   public:
    struct SubscriptionT : public InherTs... {
       public:
        SubscriptionT(ArgTs... args) : InherTs(args)... {}
    };

    typedef std::shared_ptr<SubscriptionT> SubscriptionPtr;
    typedef std::weak_ptr<SubscriptionT> SubscriptionWPtr;

    template <class ItT>
    struct Iterator {
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

        bool operator==(const Iterator& it) { return mIt == it.mIt; }
        bool operator!=(const Iterator& it) { return mIt != it.mIt; }

        SubscriptionPtr operator*() { return mIt->lock(); }

        operator bool() const { return mIt != mEnd && mIt->lock(); }

       private:
        ItT mIt, mEnd;
    };

    SubscriptionPtr subscribe(ArgTs... args) {
        SubscriptionPtr sub = std::make_shared<SubscriptionT>(args...);
        mSubscriptions.push_back(sub);
        return sub;
    };

    void subscribe(const SubscriptionPtr& sub) {
        mSubscriptions.push_back(sub);
    };

   protected:
    std::list<SubscriptionWPtr> mSubscriptions;

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
template <size_t, class, class, size_t, class, class, class...>
struct ObservableImpl;

template <size_t FCtr, size_t... FIdxs, class... FuncTs, size_t DCtr,
          size_t... DIdxs, class... DataTs, class DataT, class... Tail>
struct ObservableImpl<FCtr, Range<FIdxs...>, Wrapper<FuncTs...>, DCtr,
                      Range<DIdxs...>, Wrapper<DataTs...>, DataT, Tail...>
    : public ObservableImpl<FCtr, Range<FIdxs...>, Wrapper<FuncTs...>, DCtr + 1,
                            Range<DIdxs..., DCtr>, Wrapper<DataTs..., DataT>,
                            Tail...> {};

template <size_t FCtr, size_t... FIdxs, class... FuncTs, size_t DCtr,
          size_t... DIdxs, class... DataTs, class RetT, class... ArgTs,
          class... Tail>
struct ObservableImpl<FCtr, Range<FIdxs...>, Wrapper<FuncTs...>, DCtr,
                      Range<DIdxs...>, Wrapper<DataTs...>, RetT(ArgTs...),
                      Tail...>
    : public ObservableImpl<FCtr + 1, Range<FIdxs..., FCtr>,
                            Wrapper<FuncTs..., Func<RetT(ArgTs...)>>, DCtr,
                            Range<DIdxs...>, Wrapper<DataTs...>, Tail...> {};

template <size_t FCtr, size_t... FIdxs, class... FuncTs, size_t DCtr,
          size_t... DIdxs, class... DataTs>
struct ObservableImpl<FCtr, Range<FIdxs...>, Wrapper<FuncTs...>, DCtr,
                      Range<DIdxs...>, Wrapper<DataTs...>>
    : public ObservableImplBase<Wrapper<FuncTs..., const DataTs&...>,
                                Wrapper<SubFuncImpl<FIdxs, FuncTs>...,
                                        SubDataImpl<DIdxs, DataTs>...>> {};

// For convenience
template <class... Ts>
using Observable =
    ObservableImpl<0, Range<>, Wrapper<>, 0, Range<>, Wrapper<>, Ts...>;

// ForwardObservable
template <size_t, class, class, class...>
struct ForwardObservableImpl;

template <size_t i, class... FuncTs, class... SubTs, class... ArgTs>
struct ForwardObservableImpl<i, Wrapper<FuncTs...>, Wrapper<SubTs...>,
                             void(ArgTs...)>
    : public ObservableImplBase<
          Wrapper<FuncTs..., Func<void(ArgTs...)>>,
          Wrapper<SubTs..., SubFuncImpl<i, Func<void(ArgTs...)>>>> {
   public:
    virtual void next(ArgTs... t) {
        for (auto sub : *this) {
            call<i>(*sub, t...);
        }
    }

   protected:
    using ObservableImplBase<
        Wrapper<FuncTs..., Func<void(ArgTs...)>>,
        Wrapper<SubTs...,
                SubFuncImpl<i, Func<void(ArgTs...)>>>>::mSubscriptions;
};

template <size_t i, class... FuncTs, class... SubTs, class... ArgTs,
          class... Tail>
struct ForwardObservableImpl<i, Wrapper<FuncTs...>, Wrapper<SubTs...>,
                             void(ArgTs...), Tail...>
    : public ForwardObservableImpl<
          i + 1, Wrapper<FuncTs..., Func<void(ArgTs...)>>,
          Wrapper<SubTs..., SubFuncImpl<i, Func<void(ArgTs...)>>>, Tail...> {
    typedef ForwardObservableImpl<
        i + 1, Wrapper<FuncTs..., Func<void(ArgTs...)>>,
        Wrapper<SubTs..., SubFuncImpl<i, Func<void(ArgTs...)>>>, Tail...>
        Base;

   public:
    using Base::next;

    virtual void next(ArgTs... t) {
        for (auto sub : *this) {
            call<i>(*sub, t...);
        }
    }

   protected:
    using Base::mSubscriptions;
};

template <class... Ts>
using ForwardObservable = ForwardObservableImpl<0, Wrapper<>, Wrapper<>, Ts...>;

#endif