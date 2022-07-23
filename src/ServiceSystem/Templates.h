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

// Subscription
// Subscription Functions
template <size_t, class...>
class SubscriptionFuncImpl;

template <size_t i>
class SubscriptionFuncImpl<i> {};

template <size_t i, class RetT, class... ArgTs, class... Tail>
class SubscriptionFuncImpl<i, std::function<RetT(ArgTs...)>, Tail...>
    : public SubscriptionFuncImpl<i + 1, Tail...> {
    template <std::size_t _i, class _RetT, class... _ArgTs, class... _Tail>
    friend _RetT call(
        SubscriptionFuncImpl<_i, std::function<_RetT(_ArgTs...)>, _Tail...>&,
        _ArgTs...);

   public:
    typedef std::function<RetT(ArgTs...)> Function;

    SubscriptionFuncImpl(Function func, Tail... args)
        : SubscriptionFuncImpl<i + 1, Tail...>(args...), mFunc(func) {}

   protected:
    Function mFunc;
};

template <std::size_t i, class RetT, class... ArgTs, class... Tail>
RetT call(SubscriptionFuncImpl<i, std::function<RetT(ArgTs...)>, Tail...>& t,
          ArgTs... args) {
    return t.mFunc(args...);
}

// Subscription Data
template <size_t, class...>
class SubscriptionDataImpl;

template <size_t i>
class SubscriptionDataImpl<i> {};

template <size_t i, class DataT, class... Tail>
class SubscriptionDataImpl<i, DataT, Tail...>
    : public SubscriptionDataImpl<i + 1, Tail...> {
    template <std::size_t _i, class _DataT, class... _Tail>
    friend const _DataT& get(SubscriptionDataImpl<_i, _DataT, _Tail...>&);

    template <std::size_t _i, class _DataT, class... _Tail>
    friend void set(SubscriptionDataImpl<_i, _DataT, _Tail...>&, _DataT&&);

   public:
    SubscriptionDataImpl(DataT&& data, Tail&&... args)
        : SubscriptionDataImpl<i + 1, Tail...>(std::move<Tail>(args)...),
          mData(std::forward<DataT>(data)) {}

   protected:
    DataT mData;
};

template <std::size_t i, class DataT, class... Tail>
DataT& get(SubscriptionDataImpl<i, DataT, Tail...>& t) {
    return t.mData;
}

template <std::size_t i, class DataT, class... Tail>
void set(SubscriptionDataImpl<i, DataT, Tail...>& t, DataT&& data) {
    t.mData = std::forward<DataT>(data);
}

// Subscription Implementation
template <class... Ts>
class Subscription {};

template <class...>
class ObservableImpl;

template <class DataT, class... FuncTs, class... DataTs, class... Tail>
class ObservableImpl<Subscription<FuncTs...>, TypeWrapper<DataTs...>, DataT,
                     Tail...>
    : public ObservableImpl<Subscription<FuncTs...>,
                            TypeWrapper<DataTs..., DataT>, Tail...> {};

template <class RetT, class... ArgTs, class... FuncTs, class... DataTs,
          class... Tail>
class ObservableImpl<Subscription<FuncTs...>, TypeWrapper<DataTs...>,
                     RetT(ArgTs...), Tail...>
    : public ObservableImpl<
          Subscription<FuncTs..., std::function<RetT(ArgTs...)>>,
          TypeWrapper<DataTs...>, Tail...> {};

template <class... FuncTs, class... DataTs>
class ObservableImpl<Subscription<FuncTs...>, TypeWrapper<DataTs...>> {
   public:
    class SubscriptionT : public SubscriptionFuncImpl<0, FuncTs...>,
                          SubscriptionDataImpl<0, DataTs...> {
       public:
        SubscriptionT(FuncTs... funcs, DataTs&&... data)
            : SubscriptionFuncImpl<0, FuncTs...>(funcs...),
              SubscriptionDataImpl<0, DataTs...>(
                  std::forward<DataTs>(data)...) {}
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

// Observable
template <class... Ts>
using Observable = ObservableImpl<Subscription<>, TypeWrapper<>, Ts...>;

// ForwardObservable
template <size_t, class...>
class ForwardObservableImpl;

template <size_t i, class... SubTs, class... ArgTs>
class ForwardObservableImpl<i, Subscription<SubTs...>, void(ArgTs...)>
    : public ObservableImpl<
          Subscription<SubTs..., std::function<void(ArgTs...)>>,
          TypeWrapper<>> {
   public:
    virtual void next(ArgTs... t) {
        for (auto sub : mSubscriptions) {
            call<i>(*sub, t...);
        }
    }

   protected:
    using ObservableImpl<Subscription<SubTs..., std::function<void(ArgTs...)>>,
                         TypeWrapper<>>::mSubscriptions;
};

template <size_t i, class... SubTs, class... ArgTs, class... Tail>
class ForwardObservableImpl<i, Subscription<SubTs...>, void(ArgTs...), Tail...>
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