#ifndef SERVICE_H
#define SERVICE_H

#include <ServiceSystem/Component.h>
#include <ServiceSystem/Observable.h>

#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

template <class Base, class Derived = void, class... Ts>
struct all_base_of : std::integral_constant<
                         bool,
                         std::is_base_of<Base, Derived>::value && all_base_of<Base, Ts...>::value> {};

template <class Base>
struct all_base_of<Base, void> : std::true_type {};

template <class T1, class T2 = void, class... Ts>
struct contains : std::integral_constant<
                      bool,
                      std::is_same<T1, T2>::value || contains<T1, Ts...>::value> {};

template <class T>
struct contains<T, void> : std::false_type {};

class ServiceBase {
    friend class ServiceSystem;

   public:
    virtual ~ServiceBase() = default;

   protected:
    virtual void init();
};

template <class... Ts>
class Service : public ServiceBase {
    static_assert(all_base_of<ObservableBase, Ts...>::value,
                  "Service: Service template may only be used to specify Observable derived class");

   public:
    Service() {
        instantiate<Ts...>();
    }

    virtual ~Service() = default;

    template <class T>
    std::shared_ptr<T> Get() {
        static_assert(std::is_base_of<ObservableBase, T>::value,
                      "Service::Get(): template type is not a subclass of Observable");
        static_assert(contains<T, Ts...>::value,
                      "Service::Get(): Observable type does not exist in service");
        auto it = mObservables.find(std::type_index(typeid(T)));
        if (it != mObservables.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        throw std::runtime_error("Service::Get(): " + std::string(typeid(T).name()) +
                                 " does not exist in the service");
    }

   private:
    void init() {
        for (auto comp : mComponents) {
            comp->init();
        }
    }

    template <class... Os>
    typename std::enable_if<sizeof...(Os) == 0>::type
    instantiate() {}

    template <class O, class... Os>
    typename std::enable_if<sizeof(O) + sizeof...(Os) != 0>::type
    instantiate() {
        std::shared_ptr<O> oPtr = std::make_shared<O>();
        mObservables[std::type_index(typeid(O))] = oPtr;
        std::shared_ptr<Component> cPtr = std::dynamic_pointer_cast<Component>(oPtr);
        if (cPtr) {
            mComponents.insert(cPtr);
        }
        instantiate<Os...>();
    }

    std::unordered_map<std::type_index, std::shared_ptr<ObservableBase>> mObservables;
    std::unordered_set<std::shared_ptr<Component>> mComponents;
};

#endif
