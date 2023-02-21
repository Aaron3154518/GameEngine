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

namespace Observables {
typedef int EnumT;
enum Empty : EnumT {};

template <class T>
class DAG {
    template <class _T, class _CodeT>
    friend struct Node;

   private:
    static T& get(const Entities::UUID& eId, EnumT mCode) {
        static std::unordered_map<Entities::UUID, std::unordered_map<EnumT, T>>
            nodes;
        return nodes[eId][mCode];
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

template <class T, class CodeT>
struct Node {
    typedef T Type;
    typedef Messages::Message<CodeT, const T&> Message;

    Node(CodeT code) : mCode(code) {}
    ~Node() = default;

    const T& operator()() const { return DAG<T>::get(messager(), mCode); }
    CodeT code() const { return mCode; }

   protected:
    CodeT mCode;

    static Messages::Messager& messager() {
        static Messages::Messager MESSAGER;
        return MESSAGER;
    }

    void operator()(const T& t) const {
        auto& val = DAG<T>::get(messager(), mCode);
        val = t;
        Messages::GetMessageBus().sendMessage(Message(mCode, val));
    }
};

template <class T, class CodeT>
struct RootNode : public Node<T, CodeT> {
    using Node<T, CodeT>::Node;
    using Node<T, CodeT>::operator();
};

template <class T, class CodeT>
struct StemNode : public Node<T, CodeT> {
    using Node<T, CodeT>::Node;

    template <class F, class NodeT>
    typename std::enable_if<
        MatchSignature<F, T(typename NodeT::Type)>::value>::type
    subscribeTo(const F& func, NodeT node) {
        StemNode sn = *this;
        subscribe<NodeT>(
            &Node<T, CodeT>::messager(),
            [func, sn, node]() { sn(func(node())); }, node);
    }

    template <class... NodeTs>
    void subscribeTo(const Func<T>& func, NodeTs... args) const {
        StemNode sn = *this;
        subscribe<NodeTs...>(
            &Node<T, CodeT>::messager(), [func, sn]() { sn(func()); }, args...);
    }
};
}  // namespace Observables

#endif
