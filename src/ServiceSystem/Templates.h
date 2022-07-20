#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <tuple>

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

    // private:
    std::function<RetT(ArgTs...)> func;
};

template <std::size_t i, class... ArgTs>
struct Args {
    template <std::size_t _i, class... _ArgTs>
    friend void next(Args<_i, _ArgTs...>&, _ArgTs...);

   private:
    void next(ArgTs... args) { std::cerr << "Next: " << i << std::endl; }
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

// TODO:
// Add constructor to take all required functions and data
// Extend next() functions
// Fix std::function to take argument

#endif