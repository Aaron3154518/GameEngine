#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>

#include "../Utils/Observable/Observable.h"
#include "GameStruct.h"

// Forward declaration
template <class T>
struct ComponentFactory;

class Component {
    template <class U>
    friend class ComponentFactory;

    template <class... Us>
    friend class Service;

   public:
    virtual ~Component();

   protected:
    virtual void init(GameStruct &gs);

    Unsubscriber unsub;
};

template <class T>
class ComponentFactory {
    static_assert(std::is_base_of<Component, T>::value || std::is_same<Component, T>::value,
                  "Component Factory may only be used with Components");

   public:
    template <class... ArgTs>
    static std::unique_ptr<T> New(ArgTs... args) {
        static_assert(std::is_constructible<T, ArgTs...>::value,
                      "Component Factory cannot create Component with given arguments");

        GameStruct gs;
        std::unique_ptr<T> c = std::make_unique<T>(args...);
        ((Component *)c.get())->init(gs);
        return c;
    }
};

#endif