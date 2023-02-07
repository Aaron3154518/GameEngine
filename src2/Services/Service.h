#ifndef SERVICE_H
#define SERVICE_H

#include <Components/ComponentManager.h>

#define CONCATENATE(_1, _2) _1##_2

#define FE1(what, arg1, arg2) what(arg1, arg2)
#define FE2(what, arg1, arg2, ...) what(arg1, arg2) FE1(what, __VA_ARGS__)
#define FE3(what, arg1, arg2, ...) what(arg1, arg2) FE2(what, __VA_ARGS__)
#define FE4(what, arg1, arg2, ...) what(arg1, arg2) FE3(what, __VA_ARGS__)
#define FE5(what, arg1, arg2, ...) what(arg1, arg2) FE4(what, __VA_ARGS__)
#define FE6(what, arg1, arg2, ...) what(arg1, arg2) FE5(what, __VA_ARGS__)
#define FE7(what, arg1, arg2, ...) what(arg1, arg2) FE6(what, __VA_ARGS__)
#define FE8(what, arg1, arg2, ...) what(arg1, arg2) FE7(what, __VA_ARGS__)

#define FE_NARG(...) FE_NARG_(__VA_ARGS__, FE_RSEQ_N())
#define FE_NARG_(...) FE_ARG_N(__VA_ARGS__)
#define FE_ARG_N(_11, _12, _21, _22, _31, _32, _41, _42, _51, _52, _61, _62, \
                 _71, _72, _81, _82, N, ...)                                 \
    N
#define FE_RSEQ_N() 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0

#define FE_(N, what, x, ...) CONCATENATE(FE, N)(what, x, __VA_ARGS__)
#define FE(what, x, ...) FE_(FE_NARG(x, __VA_ARGS__), what, x, __VA_ARGS__)

#define MEMBER(t, v) t::Component* v;
#define INIT(t, v) v(&GameObjects::Get<t>()[id]),
#define SERVICE_STRUCT(name, ...)                                         \
    struct name {                                                         \
        FE(MEMBER, __VA_ARGS__)                                           \
        Entities::UUID eId;                                               \
                                                                          \
        name(const Entities::UUID& id) : FE(INIT, __VA_ARGS__) eId(id) {} \
    }

struct Empty {
    Empty(const Entities::UUID& id) {}
};

namespace Services {
template <class DataT = Empty>
class Service : public Components::ComponentManager<Components::Component> {
   protected:
    class iterator : public iterator_base {
       public:
        using iterator_base::iterator_base;

        DataT operator*() { return DataT(id()); }
    };

    iterator begin() { return iterator(mComponents.begin()); }
    iterator end() { return iterator(mComponents.end()); }
};
}  // namespace Services

#define SERVICE(name, ...)                   \
    SERVICE_STRUCT(name##Data, __VA_ARGS__); \
    class name : public Services::Service<name##Data>

#endif
