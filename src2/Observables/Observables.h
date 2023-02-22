#ifndef OBSERVABLES_H
#define OBSERVABLES_H

#include <Entities/UUID.h>
#include <Messages/Messager.h>
#include <Utils/Templates.h>

#include <functional>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

#define ROOT_NODE(name, type, enum_t) \
    struct name##RootId {};           \
    typedef Observables::RootNode<type, enum_t, name##RootId> name;
#define STEM_NODE(name, type, enum_t) \
    struct name##StemId {};           \
    typedef Observables::StemNode<type, enum_t, name##StemId> name;
#define INIT(body)                   \
    namespace {                      \
    bool _ = []() {                  \
        using namespace Observables; \
        body;                        \
        return true;                 \
    }();                             \
    }

namespace Observables {
typedef int EnumT;
enum Empty : EnumT {};

class Messager {
   private:
    template <class _T, class _CodeT, class _IdT>
    friend struct Node;

    static Messages::Messager* get() {
        static Messages::Messager MESSAGER;
        return &MESSAGER;
    }
};

template <class T>
class DAG {
    template <class _T, class _CodeT, class _IdT>
    friend struct Node;

   private:
    static T& get(const std::type_index& id, EnumT code) {
        static std::unordered_map<std::type_index, std::unordered_map<EnumT, T>>
            nodes;
        return nodes[id][code];
    }

    static T& set(const std::type_index& id, EnumT code, const T& val) {
        // std::cerr << id.name() << " " << code << std::endl;
        T& v = get(id, code);
        v = val;
        return v;
    }
};

template <class RetT>
using Func = std::function<RetT()>;

template <class... Tail>
void subscribe(Messages::Messager* m, const Func<void>& func) {
    func();
}

template <class NodeT, class F>
typename std::enable_if<
    MatchSignature<F, void(typename NodeT::Type)>::value>::type
subscribe(Messages::Messager* m, const F& func, NodeT node) {
    m->subscribeTo<typename NodeT::Message>(
        [func](const typename NodeT::Message& m) { func(m.data); },
        node.code());
    func(node());
}

template <class NodeT, class... Tail>
void subscribe(Messages::Messager* m, const Func<void>& func, NodeT node,
               Tail... args) {
    m->subscribeTo<typename NodeT::Message>([func](const auto& m) { func(); },
                                            node.code());
    subscribe<Tail...>(m, func, args...);
}

template <class T, class CodeT, class IdT>
struct Node {
    typedef CodeT Code;
    typedef T Type;
    struct Message : public Messages::Message<CodeT, const T&> {
        using Messages::Message<CodeT, const T&>::Message;
    };

    Node(CodeT code) : mCode(code) {}
    virtual ~Node() = default;

    const T& operator()() const { return DAG<T>::get(id(), mCode); }
    CodeT code() const { return mCode; }

   protected:
    CodeT mCode;

    std::type_index id() const { return std::type_index(typeid(*this)); };

    void operator()(const T& t) const {
        Messages::GetMessageBus().sendMessage(
            Message(mCode, DAG<T>::set(id(), mCode, t)));
    }
};

template <class T, class CodeT, class IdT>
struct RootNode : public Node<T, CodeT, IdT> {
    using Node<T, CodeT, IdT>::Node;
    using Node<T, CodeT, IdT>::operator();
};

template <class T, class CodeT, class IdT>
struct StemNode : public Node<T, CodeT, IdT> {
    using Node<T, CodeT, IdT>::Node;

    template <class F, class NodeT>
    typename std::enable_if<
        MatchSignature<F, T(typename NodeT::Type)>::value>::type
    subscribeTo(const F& func, NodeT node) {
        subscribe<NodeT>(
            Messager::get(),
            [func, node, id = this->id(), code = this->mCode]() {
                Messages::GetMessageBus().sendMessage(
                    Message(code, DAG<T>::set(id, code, func(node()))));
            },
            node);
    }

    template <class... NodeTs>
    void subscribeTo(const Func<T>& func, NodeTs... args) const {
        subscribe<NodeTs...>(
            Messager::get(),
            [func, id = this->id(), code = this->mCode]() {
                Messages::GetMessageBus().sendMessage(
                    Message(code, DAG<T>::set(id, code, func())));
            },
            args...);
    }
};

template <class NodeT>
void init(typename NodeT::Code c, const typename NodeT::Type& t) {
    NodeT n(c);
    n(t);
}
template <class NodeT>
void init(const std::vector<typename NodeT::Code>& cs,
          const typename NodeT::Type& t) {
    for (auto c : cs) {
        init<NodeT>(c, t);
    }
}
}  // namespace Observables

#endif
