#ifndef TARGET_OBSERVABLE_H
#define TARGET_OBSERVABLE_H

#include <type_traits>
#include <unordered_map>

#include "../src/ServiceSystem/Observable.h"

// Observable for triggering events for a specific type
template <class IdT, class... Ts>
class TargetObservable : public ObservableBase {
   public:
    typedef std::function<void(const Ts&...)> IdSubscriptionFunc;
    typedef std::function<void(IdT, const Ts&...)> AllSubscriptionFunc;

    class IdObservable : public Observable<void(const Ts&...), IdT> {
        friend class TargetObservable<IdT, Ts...>;
    };
    class AllObservable : public Observable<void(IdT, const Ts&...)> {
        friend class TargetObservable<IdT, Ts...>;
    };

    typedef typename IdObservable::SubscriptionPtr IdSubscriptionPtr;
    typedef typename AllObservable::SubscriptionPtr AllSubscriptionPtr;

    enum : size_t { FUNC = 0, ID };

    virtual ~TargetObservable() = default;

    IdSubscriptionPtr subscribe(IdSubscriptionFunc func, IdT id) {
        return mIdObservable.subscribe(func, id);
    }

    AllSubscriptionPtr subscribeToAll(AllSubscriptionFunc func) {
        return mAllObservable.subscribe(func);
    }

    virtual void next(IdT target, const Ts&... args) {
        for (auto sub : mIdObservable) {
            if (sub->template get<ID>() == target) {
                sub->template get<FUNC>()(args...);
            }
        }
        for (auto sub : mAllObservable) {
            sub->template get<FUNC>()(target, args...);
        }
    }

   private:
    IdObservable mIdObservable;
    AllObservable mAllObservable;
};

template <class IdT, class T>
class TargetDataObservable : public TargetObservable<IdT, T> {
   public:
    typedef TargetObservable<IdT, T> Base;
    using typename Base::AllSubscriptionFunc;
    using typename Base::AllSubscriptionPtr;
    using typename Base::IdSubscriptionFunc;
    using typename Base::IdSubscriptionPtr;

    virtual ~TargetDataObservable() = default;

    IdSubscriptionPtr subscribe(IdSubscriptionFunc func, IdT id) {
        func(mData[id]);
        return Base::subscribe(func, id);
    }

    AllSubscriptionPtr subscribeToAll(AllSubscriptionFunc func) {
        for (auto pair : mData) {
            func(pair.first, pair.second);
        }
        return Base::subscribeToAll(func);
    }

    virtual void next(IdT target, const T& t) {
        mData[target] = t;
        Base::next(target, t);
    }

    const T& get(IdT target) { return mData[target]; }

   private:
    std::unordered_map<IdT, T> mData;
};

#endif
