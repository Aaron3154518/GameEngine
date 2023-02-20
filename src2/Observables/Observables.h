#ifndef OBSERVABLES_H
#define OBSERVABLES_H

#include <Entities/UUID.h>
#include <Messages/Messager.h>

#include <functional>
#include <initializer_list>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace Observables {
// Use normal message bus for external subscriptions
// Use specialized message bus for internal subscriptions
// (use the fact that all nodes have the same lifecycle)
// (make it a messager)
Messages::MessageBus& GetMessageBus();
typedef int EnumT;

enum Empty : EnumT {};

template <class T>
class DAG {
    template <class _T, class _CodeT>
    friend struct Node;

   private:
    static T& get(const Entities::UUID& eId, EnumT mCode) {
        static DAG dag;
        return dag.mNodes[eId][mCode];
    }

    std::unordered_map<Entities::UUID, std::unordered_map<EnumT, T>> mNodes;
};

template <class RetT>
using Func = std::function<RetT()>;

template <class... Tail>
void subscribe(Messages::Messager* m, const Func<void>& func) {
    func();
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
    typedef Messages::Message<CodeT, const T&> Message;

    Node(CodeT code) : mCode(code) {}
    ~Node() = default;

    T& operator()() const { return DAG<T>::get(messager(), mCode); }
    CodeT code() const { return mCode; }

   protected:
    CodeT mCode;

    static Messages::Messager& messager() {
        static Messages::Messager MESSAGER;
        return MESSAGER;
    }

    void set(const T& t) const {
        auto& val = operator()();
        val = t;
        Messages::GetMessageBus().sendMessage(Message(mCode, val));
    }
};

template <class T, class CodeT>
struct RootNode : public Node<T, CodeT> {
    using Node<T, CodeT>::Node;
    using Node<T, CodeT>::set;
};

template <class T, class CodeT>
struct StemNode : public Node<T, CodeT> {
    using Node<T, CodeT>::Node;

    template <class... NodeTs>
    void subscribeTo(const Func<T>& func, NodeTs... args) const {
        StemNode sn = *this;
        subscribe<NodeTs...>(
            &Node<T, CodeT>::messager(), [func, sn]() { sn.set(func()); },
            args...);
    }
};
}  // namespace Observables

#endif
