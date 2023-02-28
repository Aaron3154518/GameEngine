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
    typedef Observables::RootNode<type, enum_t> name;
#define STEM_NODE(name, type, enum_t) \
    typedef Observables::StemNode<type, enum_t> name;
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
    template <class _T>
    friend class Node;

    static Messages::Messager* get() {
        static Messages::Messager MESSAGER;
        return &MESSAGER;
    }
};

template <class T>
class DAG {
    template <class>
    friend class Node;
    template <class>
    friend class RootNodeBase;

   private:
    static T& get(const std::type_index& id, EnumT code) {
        static std::unordered_map<std::type_index, std::unordered_map<EnumT, T>>
            nodes;
        return nodes[id][code];
    }

    static T& set(const std::type_index& id, EnumT code, const T& val) {
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
    m->subscribeTo<NodeT>([node, func](const auto& m) { func(node()); },
                          node.code);
    func(node());
}

template <class NodeT, class... Tail>
void subscribe(Messages::Messager* m, const Func<void>& func, NodeT node,
               Tail... args) {
    m->subscribeTo<NodeT>([func](const auto& m) { func(); }, node.code);
    subscribe<Tail...>(m, func, args...);
}

template <class T>
class Node : public Messages::Message<EnumT> {
   public:
    typedef T Type;
    typedef std::function<const T&()> GetFunc;
    typedef std::function<void(const T&)> SetFunc;

    virtual ~Node() = default;

    const T& operator()() const { return DAG<T>::get(id(), code); }

    GetFunc getter() const {
        return [id = this->id(), code = this->code]() -> const T& {
            return DAG<T>::get(id, code);
        };
    }
    virtual SetFunc setter() const {
        return [](const T& t) {};
    }

   protected:
    using Messages::Message<EnumT>::Message;

    void operator()(const T& t) const {
        DAG<T>::set(id(), code, t);
        Messages::GetMessageBus().sendMessage(*this);
    }
};

template <class T>
class RootNodeBase : public Node<T> {
   public:
    using Node<T>::operator();

    typename Node<T>::SetFunc setter() const {
        return [id = this->id(), code = this->code](const T& t) {
            DAG<T>::set(id, code, t);
            RootNodeBase(id, code)(t);
        };
    }

   protected:
    using Node<T>::Node;
};

template <class T, class CodeT>
class RootNode : public RootNodeBase<T> {
   public:
    typedef CodeT NodeCode;

    RootNode(CodeT code) : RootNodeBase<T>(code) {}
};

template <class T>
class StemNodeBase : public Node<T> {
   public:
    typename Node<T>::SetFunc setter() const {
        return [](const T& t) {
            std::cerr << "StemNode::setter(): Cannot call setter on Stem Node"
                      << std::endl;
        };
    }

    template <class F, class NodeT>
    typename std::enable_if<
        MatchSignature<F, T(typename NodeT::Type)>::value>::type
    subscribeTo(const F& func, NodeT node) const {
        subscribe<NodeT>(
            Messager::get(),
            [func, node, id = this->id(), code = this->code]() {
                Messages::GetMessageBus().sendMessage(
                    Message(code, DAG<T>::set(id, code, func(node()))));
            },
            node);
    }

    template <class... NodeTs>
    void subscribeTo(const Func<T>& func, NodeTs... args) const {
        subscribe<NodeTs...>(
            Messager::get(),
            [func, id = this->id(), code = this->code]() {
                Messages::GetMessageBus().sendMessage(
                    Message(code, DAG<T>::set(id, code, func())));
            },
            args...);
    }

   protected:
    using Node<T>::Node;
};

template <class T, class CodeT>
class StemNode : public StemNodeBase<T> {
   public:
    typedef CodeT NodeCode;

    StemNode(CodeT code) : StemNodeBase<T>(code) {}
};

template <class T>
struct NodeWrapper {
   public:
   private:
    std::unique_ptr<Node<T>> mNode;
};

template <class NodeT>
void init(typename NodeT::NodeCode c, const typename NodeT::Type& t) {
    NodeT n(c);
    n(t);
}
template <class NodeT>
void init(const std::vector<typename NodeT::NodeCode>& cs,
          const typename NodeT::Type& t) {
    for (auto c : cs) {
        init<NodeT>(c, t);
    }
}
}  // namespace Observables

#endif
