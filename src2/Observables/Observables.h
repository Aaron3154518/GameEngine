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

template <class T>
class DAG {
    template <class _T, class _RootT, class _StemT>
    friend struct Access;

   public:
    static DAG& Get() {
        static DAG dag;
        return dag;
    }

   private:
    T& get(const Entities::UUID& eId, EnumT code) { return mNodes[eId][code]; }

    std::unordered_map<Entities::UUID, std::unordered_map<EnumT, T>> mNodes;
};

template <class T, class CodeT>
struct Node {
    typedef Messages::Message<CodeT, const T&> Message;

    CodeT code;
};

// Each variables has Owner (eid), Type (T), Name (EnumT)
template <class T, class RootT, class StemT>
struct Access {
    typedef typename Node<T, RootT>::Message RootMessage;
    typedef typename Node<T, StemT>::Message StemMessage;

    static const Entities::UUID& id() {
        static const Entities::UUID& ID = Entities::generateUUID();
        return ID;
    }

    Node<T, RootT> operator[](RootT code) const { return {code}; }
    Node<T, StemT> operator[](StemT code) const { return {code}; }

    T& get(RootT code) const { return DAG<T>::Get().get(id(), code); }
    T& get(StemT code) const { return DAG<T>::Get().get(id(), code); }

    void set(RootT code, const T& t) const {
        auto& val = get(code);
        val = t;
        Messages::GetMessageBus().sendMessage(RootMessage(code, val));
    }
};

typedef std::function<void()> SubFunc;

template <class... Tail>
typename std::enable_if<sizeof...(Tail) == 0>::type subscribe(
    Messages::Messager* m, const SubFunc& func) {
    func();
}

template <class NodeT, class... Tail>
typename std::enable_if<sizeof(NodeT) + sizeof...(Tail) != 0>::type subscribe(
    Messages::Messager* m, const SubFunc& func, NodeT node, Tail... args) {
    m->subscribeTo<typename NodeT::Message>([func](const auto& m) { func(); },
                                            node.code);
    subscribe<Tail...>(m, func, args...);
}
}  // namespace Observables

#endif
