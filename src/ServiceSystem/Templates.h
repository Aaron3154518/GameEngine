#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

// Helper classes
template <class...>
struct TypeWrapper;

template <std::size_t i, class...>
struct Func;

template <std::size_t i, class RetT, class... ArgTs>
struct Func<i, RetT(ArgTs...)> {
    template <std::size_t _i, class _RetT, class... _ArgTs>
    friend std::function<_RetT(_ArgTs...)>& setFunc(
        Func<_i, _RetT(_ArgTs...)>&);

    template <std::size_t _i, class _RetT, class... _ArgTs>
    friend _RetT call(Func<_i, _RetT(_ArgTs...)>&, _ArgTs...);

   private:
    std::function<RetT(ArgTs...)> func;
};

template <std::size_t i, class... ArgTs>
struct Args {
    template <std::size_t _i, class... _ArgTs>
    friend void next(Args<_i, _ArgTs...>&, _ArgTs...);

   private:
    virtual void next(ArgTs... args) {
        std::cerr << "Next: " << i << std::endl;
    }
};

template <std::size_t i, class DataT>
struct Data {
    template <std::size_t _i, class _DataT>
    friend const std::shared_ptr<_DataT>& get(Data<_i, _DataT>&);

    template <std::size_t _i, class _DataT>
    friend void set(Data<_i, _DataT>&, std::shared_ptr<_DataT>);

   private:
    std::shared_ptr<DataT> mData;
};

// Implementation class, i = Func counter, j = Args counter, k = Data counter
template <std::size_t i, std::size_t j, std::size_t k, class...>
struct Impl;

// Base
template <std::size_t i, std::size_t j, std::size_t k>
struct Impl<i, j, k> {};

// Function
template <std::size_t i, std::size_t j, std::size_t k, class RetT,
          class... ArgTs, class... Tail>
struct Impl<i, j, k, RetT(ArgTs...), Tail...>
    : public Func<i, RetT(ArgTs...)>, public Impl<i + 1, j, k, Tail...> {};

// Arguments
template <std::size_t i, std::size_t j, std::size_t k, class... ArgTs,
          class... Tail>
struct Impl<i, j, k, TypeWrapper<ArgTs...>, Tail...>
    : public Args<j, ArgTs...>, public Impl<i, j + 1, k, Tail...> {};

// Data
template <std::size_t i, std::size_t j, std::size_t k, class DataT,
          class... Tail>
struct Impl<i, j, k, DataT, Tail...> : public Data<k, DataT>,
                                       public Impl<i, j, k + 1, Tail...> {};

template <class... Ts>
using Model = Impl<0, 0, 0, Ts...>;

/*
template <std::size_t i, class RetT, class... ArgTs, class... Tail>
void set(Impl<i, RetT(ArgTs...), Tail...>& t,
         std::function<RetT(ArgTs...)> func) {
    t.Func<i, RetT(ArgTs...)>::func = func;
}

template <std::size_t i, class RetT, class... ArgTs, class... Tail>
RetT call(Impl<i, RetT(ArgTs...), Tail...>& t, ArgTs... args) {
    return t.Func<i, RetT(ArgTs...)>::func(args...);
}

template <std::size_t i, class... ArgTs, class... Tail>
void next(Impl<i, TypeWrapper<ArgTs...>, Tail...>& t, ArgTs... args) {
    t.Func<i, TypeWrapper<ArgTs...>>::next(args...);
}

template <std::size_t i, class DataT, class... Tail>
const std::shared_ptr<DataT>& get(Impl<i, DataT, Tail...>& t) {
    return t.Data<i, DataT>::get();
}*/

template <std::size_t i, class RetT, class... ArgTs>
std::function<RetT(ArgTs...)>& setFunc(Func<i, RetT(ArgTs...)>& t) {
    return t.func;
}

template <std::size_t i, class RetT, class... ArgTs>
RetT call(Func<i, RetT(ArgTs...)>& t, ArgTs... args) {
    return t.func(args...);
}

template <std::size_t i, class... ArgTs>
void next(Args<i, ArgTs...>& t, ArgTs... args) {
    t.next(args...);
}

template <std::size_t i, class DataT>
const std::shared_ptr<DataT>& get(Data<i, DataT>& t) {
    return t.mData;
}

template <std::size_t i, class DataT>
void set(Data<i, DataT>& t, std::shared_ptr<DataT> data) {
    t.mData = data;
}

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

/*
template <class, size_t, size_t>
struct RangeImpl;

template <size_t Start, size_t End, size_t... Prev>
struct RangeImpl<Range<Prev...>, Start, End> {
    static_assert(Start < End, "Range: index out of range");

    typedef
        typename RangeImpl<Range<Prev..., Start>, Start + 1, End>::range range;
};

template <size_t End, size_t... Prev>
struct RangeImpl<Range<Prev...>, End, End> {
    typedef Range<Prev...> range;
};

template <size_t Start, size_t End>
using RangeGen = RangeImpl<Range<>, Start, End>;
*/

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
struct Subscription {};

// For storing different observables
struct ObservableBase {};

template <size_t, class, class, size_t, class, class, class...>
struct ObservableImpl;

template <size_t FCtr, size_t... FIdxs, class... FuncTs, size_t DCtr,
          class... DataTs, size_t... DIdxs, class DataT, class... Tail>
struct ObservableImpl<FCtr, Range<FIdxs...>, Subscription<FuncTs...>, DCtr,
                      Range<DIdxs...>, TypeWrapper<DataTs...>, DataT, Tail...>
    : public ObservableImpl<FCtr, Range<FIdxs...>, Subscription<FuncTs...>,
                            DCtr + 1, Range<DIdxs..., DCtr>,
                            TypeWrapper<DataTs..., DataT>, Tail...> {};

template <size_t FCtr, size_t... FIdxs, class... FuncTs, size_t DCtr,
          class... DataTs, size_t... DIdxs, class RetT, class... ArgTs,
          class... Tail>
struct ObservableImpl<FCtr, Range<FIdxs...>, Subscription<FuncTs...>, DCtr,
                      Range<DIdxs...>, TypeWrapper<DataTs...>, RetT(ArgTs...),
                      Tail...>
    : public ObservableImpl<
          FCtr + 1, Range<FIdxs..., FCtr>,
          Subscription<FuncTs..., std::function<RetT(ArgTs...)>>, DCtr,
          Range<DIdxs...>, TypeWrapper<DataTs...>, Tail...> {};

template <size_t FCtr, size_t... FIdxs, class... FuncTs, size_t DCtr,
          size_t... DIdxs, class... DataTs>
struct ObservableImpl<FCtr, Range<FIdxs...>, Subscription<FuncTs...>, DCtr,
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
    std::vector<SubscriptionPtr> mSubscriptions;
};

template <class... Ts>
using Observable = ObservableImpl<0, Range<>, Subscription<>, 0, Range<>,
                                  TypeWrapper<>, Ts...>;

// ForwardObservable
template <class... SubTs>
using ForwardObservableBase =
    ObservableImpl<sizeof...(SubTs), RangeGen<0, sizeof...(SubTs)>,
                   Subscription<SubTs...>, 0, Range<>, TypeWrapper<>>;

template <size_t, class...>
struct ForwardObservableImpl;

template <size_t i, class... SubTs, class... ArgTs>
struct ForwardObservableImpl<i, Subscription<SubTs...>, void(ArgTs...)>
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
struct ForwardObservableImpl<i, Subscription<SubTs...>, void(ArgTs...), Tail...>
    : public ForwardObservableImpl<
          i + 1, Subscription<SubTs..., std::function<void(ArgTs...)>>,
          Tail...> {
    typedef ForwardObservableImpl<
        i + 1, Subscription<SubTs..., std::function<void(ArgTs...)>>, Tail...>
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
using ForwardObservable = ForwardObservableImpl<0, Subscription<>, Ts...>;

// Testing
template <size_t, class>
class T {};

class B {
   public:
    B(int i) { std::cerr << "B" << i << std::endl; }
};

class C {
   public:
    C(int i) { std::cerr << "C" << i << std::endl; }
};

class D {
   public:
    D(int i) { std::cerr << "D" << i << std::endl; }
};

template <class...>
class AImpl2;

template <size_t... is, class... Ts>
class AImpl2<T<is, Ts>...> : public Ts... {
   public:
    AImpl2() : Ts(is)... { std::cerr << "A" << std::endl; }
};

template <size_t, class...>
class AImpl;

template <size_t i, class U, class... Ts, class... Tail>
class AImpl<i, TypeWrapper<Ts...>, U, Tail...>
    : public AImpl<i + 1, TypeWrapper<Ts..., T<i, U>>, Tail...> {};

template <size_t i, class... Ts>
class AImpl<i, TypeWrapper<Ts...>> : public AImpl2<Ts...> {};

template <class... Ts>
using A = AImpl<0, TypeWrapper<>, Ts...>;

template <class... Ts>
class VarFuncs : public VarFuncs<Ts>... {
   public:
    VarFuncs() { std::cerr << "Hello There" << std::endl; }
};

template <class RetT, class... ArgTs>
class VarFuncs<RetT(ArgTs...)> {
   public:
    VarFuncs() { std::cerr << "Sup" << std::endl; }
};

void foo() {
    VarFuncs<void(int), int(std::string), bool(bool), int(int),
             void(std::string)>
        v;
}

// TODO:
// Variadic Inheritance
// Data
// Unsubscribe

#endif