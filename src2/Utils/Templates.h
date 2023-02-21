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

#endif
