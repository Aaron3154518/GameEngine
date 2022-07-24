#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <memory>

// Range
template <size_t...>
struct Range {};

template <bool, size_t, class>
struct Merge;

template <size_t Off, size_t Start, size_t... Tail>
struct Merge<false, Off, Range<Start, Tail...>> {
    typedef Range<Start, Tail..., (Off + Tail)...> range;
};

template <size_t Off, size_t... Idxs>
struct Merge<true, Off, Range<Idxs...>> {
    typedef Range<Idxs..., (Off + Idxs)...> range;
};

template <size_t Start, size_t End>
struct RangeGenImpl {
    static_assert(Start < End, "Invalid start");

    typedef typename Merge<
        (End - Start + 1) % 2 == 0, (End - Start + 1) / 2,
        typename RangeGenImpl<Start, Start + (End - Start) / 2>::range>::range
        range;
};

template <size_t End>
struct RangeGenImpl<End, End> {
    typedef Range<End> range;
};

template <size_t Start, size_t End>
using RangeGen = typename RangeGenImpl<Start, End - 1>::range;

// Subscription
// Subscription Functions
template <size_t, class...>
struct SubscriptionFuncImpl;

template <size_t i, class RetT, class... ArgTs>
struct SubscriptionFuncImpl<i, std::function<RetT(ArgTs...)>> {
    template <std::size_t _i, class _RetT, class... _ArgTs>
    friend _RetT call(
        SubscriptionFuncImpl<_i, std::function<_RetT(_ArgTs...)>>&, _ArgTs...);

   public:
    typedef std::function<RetT(ArgTs...)> Function;

    SubscriptionFuncImpl(Function func) : mFunc(func) {}

   protected:
    Function mFunc;
};

template <std::size_t i, class RetT, class... ArgTs>
RetT call(SubscriptionFuncImpl<i, std::function<RetT(ArgTs...)>>& t,
          ArgTs... args) {
    return t.mFunc(args...);
}

// Subscription Data
template <size_t, class...>
struct SubscriptionDataImpl;

template <size_t i, class DataT>
struct SubscriptionDataImpl<i, DataT> {
    template <std::size_t _i, class _DataT>
    friend const _DataT& get(SubscriptionDataImpl<_i, _DataT>&);

    template <std::size_t _i, class _DataT>
    friend void set(SubscriptionDataImpl<_i, _DataT>&, _DataT&&);

   public:
    SubscriptionDataImpl(DataT&& data) : mData(std::forward<DataT>(data)) {}

   protected:
    DataT mData;
};

template <std::size_t i, class DataT>
DataT& get(SubscriptionDataImpl<i, DataT>& t) {
    return t.mData;
}

template <std::size_t i, class DataT>
void set(SubscriptionDataImpl<i, DataT>& t, DataT&& data) {
    t.mData = std::forward<DataT>(data);
}

// Observable
template <class... Ts>
struct TypeWrapper {};

// For storing different observables
struct ObservableBase {};

template <size_t, class, class, size_t, class, class, class...>
struct ObservableImpl;

template <size_t FCtr, size_t... FIdxs, class... FuncTs, size_t DCtr,
          class... DataTs, size_t... DIdxs, class DataT, class... Tail>
struct ObservableImpl<FCtr, Range<FIdxs...>, TypeWrapper<FuncTs...>, DCtr,
                      Range<DIdxs...>, TypeWrapper<DataTs...>, DataT, Tail...>
    : public ObservableImpl<FCtr, Range<FIdxs...>, TypeWrapper<FuncTs...>,
                            DCtr + 1, Range<DIdxs..., DCtr>,
                            TypeWrapper<DataTs..., DataT>, Tail...> {};

template <size_t FCtr, size_t... FIdxs, class... FuncTs, size_t DCtr,
          class... DataTs, size_t... DIdxs, class RetT, class... ArgTs,
          class... Tail>
struct ObservableImpl<FCtr, Range<FIdxs...>, TypeWrapper<FuncTs...>, DCtr,
                      Range<DIdxs...>, TypeWrapper<DataTs...>, RetT(ArgTs...),
                      Tail...>
    : public ObservableImpl<
          FCtr + 1, Range<FIdxs..., FCtr>,
          TypeWrapper<FuncTs..., std::function<RetT(ArgTs...)>>, DCtr,
          Range<DIdxs...>, TypeWrapper<DataTs...>, Tail...> {};

template <size_t FCtr, size_t... FIdxs, class... FuncTs, size_t DCtr,
          size_t... DIdxs, class... DataTs>
struct ObservableImpl<FCtr, Range<FIdxs...>, TypeWrapper<FuncTs...>, DCtr,
                      Range<DIdxs...>, TypeWrapper<DataTs...>>
    : public ObservableBase {
   public:
    struct SubscriptionT : public SubscriptionFuncImpl<FIdxs, FuncTs>...,
                           SubscriptionDataImpl<DIdxs, DataTs>... {
       public:
        SubscriptionT(FuncTs... funcs, DataTs&&... data)
            : SubscriptionFuncImpl<FIdxs, FuncTs>(funcs)...,
              SubscriptionDataImpl<DIdxs, DataTs>(
                  std::forward<DataTs>(data))... {}
    };

    typedef std::shared_ptr<SubscriptionT> SubscriptionPtr;

    SubscriptionPtr subscribe(FuncTs... funcs, DataTs&&... data) {
        mSubscriptions.push_back(std::make_shared<SubscriptionT>(
            funcs..., std::forward<DataTs>(data)...));
        return mSubscriptions.back();
    };

   protected:
    std::list<SubscriptionPtr> mSubscriptions;
};

template <class... Ts>
using Observable = ObservableImpl<0, Range<>, TypeWrapper<>, 0, Range<>,
                                  TypeWrapper<>, Ts...>;

// ForwardObservable
template <class... SubTs>
using ForwardObservableBase =
    ObservableImpl<sizeof...(SubTs), RangeGen<0, sizeof...(SubTs)>,
                   TypeWrapper<SubTs...>, 0, Range<>, TypeWrapper<>>;

template <size_t, class...>
struct ForwardObservableImpl;

template <size_t i, class... SubTs, class... ArgTs>
struct ForwardObservableImpl<i, TypeWrapper<SubTs...>, void(ArgTs...)>
    : public ForwardObservableBase<SubTs..., std::function<void(ArgTs...)>> {
   public:
    virtual void next(ArgTs... t) {
        for (auto sub : mSubscriptions) {
            call<i>(*sub, t...);
        }
    }

   protected:
    using ForwardObservableBase<SubTs...,
                                std::function<void(ArgTs...)>>::mSubscriptions;
};

template <size_t i, class... SubTs, class... ArgTs, class... Tail>
struct ForwardObservableImpl<i, TypeWrapper<SubTs...>, void(ArgTs...), Tail...>
    : public ForwardObservableImpl<
          i + 1, TypeWrapper<SubTs..., std::function<void(ArgTs...)>>,
          Tail...> {
    typedef ForwardObservableImpl<
        i + 1, TypeWrapper<SubTs..., std::function<void(ArgTs...)>>, Tail...>
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
using ForwardObservable = ForwardObservableImpl<0, TypeWrapper<>, Ts...>;

// TODO:
// Variadic Inheritance
// Data
// Unsubscribe

#endif