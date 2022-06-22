#ifndef SERVICE_HANDLER_H
#define SERVICE_HANDLER_H

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

class Service {};

class ServiceHandler {
   public:
    template <class T>
    static std::shared_ptr<T> Get() {
        static_assert(std::is_base_of<Service, T>::value,
                      "ServiceHandler::Get(): template type is not a subclass of Service");
        auto it = Services().find(std::type_index(typeid(T)));
        if (it != Services().end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        throw std::runtime_error("ServiceHandler::Get(): " + std::string(typeid(T).name()) +
                                 " is not a registered service");
    }

    template <class T>
    static bool Add() {
        static_assert(std::is_base_of<Service, T>::value,
                      "ServiceHandler::Add(): template type is not a subclass of Service");
        static_assert(std::is_constructible<T>::value,
                      "ServiceHandler::Add(): the template type must be default constructible");
        // std::cerr << "Add: " << std::type_index(typeid(T)).name() << std::endl;
        Services()[std::type_index(typeid(T))] = std::make_shared<T>();
        return true;
    }

   private:
    ServiceHandler() = delete;
    ~ServiceHandler() = delete;

    static std::unordered_map<std::type_index, std::shared_ptr<Service>>& Services();
};

template <class T>
class RegisterService { static bool _; };

template <class T>
bool RegisterService<T>::_ = ServiceHandler::Add<T>();

#define REGISTER_SERVICE(type) \
    template class RegisterService<type>

#endif
