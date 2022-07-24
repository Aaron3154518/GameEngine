#ifndef TEMPLATES_H
#define TEMPLATES_H

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

// Subscription
// Subscription Functions
template <size_t, class...>
struct SubFuncImpl;

template <size_t i, class RetT, class... ArgTs>
struct SubFuncImpl<i, Func<RetT(ArgTs...)>> {
    template <std::size_t _i, class _RetT, class... _ArgTs>
    friend _RetT call(SubFuncImpl<_i, Func<_RetT(_ArgTs...)>>&, _ArgTs...);

   public:
    typedef Func<RetT(ArgTs...)> Function;

    SubFuncImpl(Function func) : mFunc(func) {}

   protected:
    Function mFunc;
};

template <std::size_t i, class RetT, class... ArgTs>
RetT call(SubFuncImpl<i, Func<RetT(ArgTs...)>>& t, ArgTs... args) {
    return t.mFunc(args...);
}

// Subscription Data
template <size_t i, class DataT>
struct SubDataImpl {
    template <std::size_t _i, class _DataT>
    friend _DataT& get(SubDataImpl<_i, _DataT>&);

    template <std::size_t _i, class _DataT>
    friend void set(SubDataImpl<_i, _DataT>&, const _DataT&);

   public:
    SubDataImpl(const DataT& data) : mData(data) {}

   protected:
    DataT mData;
};

template <std::size_t i, class DataT>
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

    SubscriptionPtr subscribe(ArgTs... args) {
        mSubscriptions.push_back(std::make_shared<SubscriptionT>(args...));
        return mSubscriptions.back();
    };

   protected:
    std::list<SubscriptionPtr> mSubscriptions;
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
        for (auto sub : mSubscriptions) {
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
        for (auto sub : mSubscriptions) {
            call<i>(*sub, t...);
        }
    }

   protected:
    using Base::mSubscriptions;
};

template <class... Ts>
using ForwardObservable = ForwardObservableImpl<0, Wrapper<>, Wrapper<>, Ts...>;

// TODO:
// Unsubscribe

#endif