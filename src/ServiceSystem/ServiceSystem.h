#ifndef SERVICE_SYSTEM_H
#define SERVICE_SYSTEM_H

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "ServiceSystem/Component.h"
#include "ServiceSystem/Service.h"

class ServiceSystem {
   public:
    template <class T>
    static std::shared_ptr<T> Get() {
        static_assert(std::is_base_of<ServiceBase, T>::value,
                      "ServiceSystem::Get(): template type is not a subclass of Service");
        auto it = Services().find(std::type_index(typeid(T)));
        if (it != Services().end()) {
            return std::static_pointer_cast<T>(it->second);
        } else {
            return Add<T>();
        }
    }

   private:
    ServiceSystem() = delete;
    ~ServiceSystem() = delete;

    template <class T>
    static std::shared_ptr<T> Add() {
        static_assert(std::is_base_of<ServiceBase, T>::value,
                      "ServiceSystem::Add(): template type is not a subclass of Service");
        static_assert(!std::is_base_of<Component, T>::value,
                      "ServiceSystem::Add(): Services cannot be Components, use Component-type member variables instead");
        static_assert(std::is_constructible<T>::value,
                      "ServiceSystem::Add(): the Service must be default constructible");
        std::shared_ptr<T> t = std::make_shared<T>();
        Services()[std::type_index(typeid(T))] = t;
        ((ServiceBase*)t.get())->init();
        return t;
    }

    static std::unordered_map<std::type_index, std::shared_ptr<ServiceBase>>& Services();
};

#endif
