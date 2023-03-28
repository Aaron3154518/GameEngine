#ifndef TEMPLATES_H
#define TEMPLATES_H

// Match type to function signature
template <class...>
struct MatchSignature;

template <class F, class RetT, class... ArgTs>
struct MatchSignature<F, RetT(ArgTs...)> {
    enum {
        value = std::is_convertible<F, std::function<RetT(ArgTs...)>>::value
    };
};

// Class for joining enums
template <class... Ts>
union JoinImpl {};

template <class T, class... Tail>
union JoinImpl<T, Tail...> {
    T v;
    JoinImpl<Tail...> j;
};

template <class... Ts>
struct JoinBase {
   public:
    template <class T>
    JoinBase& operator=(T val) {
        set(val);
        return *this;
    }

    template <class T>
    bool operator==(T v) const {
        return v == get<T>() && t == idx<T>();
    }

    template <class... _Ts>
    friend std::ostream& operator<<(std::ostream& os,
                                    const JoinBase<_Ts...>& j) {
        os << j.j.v << " " << j.t;
        return os;
    }

   protected:
    template <class T, class Ti, class... Tail>
    static typename std::enable_if<std::is_same<T, Ti>::value, int>::type
    idx() {
        return 0;
    }

    template <class T, class Ti, class... Tail>
    static typename std::enable_if<!std::is_same<T, Ti>::value, int>::type
    idx() {
        return idx<T, Tail...>() + 1;
    }

    template <class T>
    int idx() const {
        return idx<T, Ts...>();
    }

    template <class T, class... Tail>
    static T get(const JoinImpl<T, Tail...>& j) {
        return j.v;
    }

    template <class T, class Ti, class... Tail>
    static typename std::enable_if<!std::is_same<T, Ti>::value, T>::type get(
        const JoinImpl<Ti, Tail...>& j) {
        return get<T>(j.j);
    }

    template <class T>
    T get() const {
        return get<T>(j);
    }

    template <class Ti, class... Tail>
    static int set(JoinImpl<Ti, Tail...>& j, Ti v) {
        j.v = v;
        return 0;
    }

    template <class T, class Ti, class... Tail>
    static int set(JoinImpl<Ti, Tail...>& j, T v) {
        return set(j.j, v) + 1;
    }

    template <class T>
    void set(T v) {
        t = set(j, v);
    }

   private:
    JoinImpl<Ts...> j;
    int t = 0;
};

template <class J, class... Ts>
struct JoinConstructor : public J {};

template <class J, class T, class... Tail>
struct JoinConstructor<J, T, Tail...> : public JoinConstructor<J, Tail...> {
    using JoinConstructor<J, Tail...>::JoinConstructor;
    JoinConstructor(T val) { this->set(val); }
};

template <class... Ts>
struct Join : public JoinConstructor<JoinBase<Ts...>, Ts...> {
    using JoinConstructor<JoinBase<Ts...>, Ts...>::JoinConstructor;
};

#endif
