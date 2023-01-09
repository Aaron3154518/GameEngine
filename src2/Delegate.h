#ifndef DELEGATE_H
#define DELEGATE_H

// TODO: arbitrary captures

template <class...>
class Delegate;

template <class RetT, class... ArgTs>
class Delegate<RetT(ArgTs...)> {
    typedef void (*FuncType)(void* obj, ArgTs... args);

   public:
    template <class T, void (T::*TMethod)(ArgTs...)>
    static Delegate create(T* obj) {
        mObj = obj;
        mFunc = &call<T, TMethod>;
    }

    RetT operator()(ArgTs... args) const { return mFunc(mObs, args...); }

   private:
    void* mObj;
    FuncType mFunc;

    template <class T, void (T::*TMethod)(ArgTs...)>
    static RetT call(void* obj, ArgTs... args) {
        return (static_cast<T*>(objs)->TMethod)(args...);
    }
};

#endif
