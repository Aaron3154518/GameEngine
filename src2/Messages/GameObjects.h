#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <Entities/UUID.h>
#include <Messages/Messager.h>

#include <memory>
#include <typeindex>
#include <unordered_map>

class GameObjects {
   public:
    template <class T>
    static T& Get() {
        static_assert(std::is_base_of<Messages::Messager, T>::value,
                      "Get(): Object type must inherit from Messager");
        auto& ptr = GetSingletons()[std::type_index(typeid(T))];
        if (!ptr) {
            ptr = std::make_unique<T>();
            ptr->init();
        }
        return static_cast<T&>(*ptr);
    }

    template <class T, int Id>
    static T& Get() {
        static_assert(std::is_base_of<Messages::Messager, T>::value,
                      "Get(): Object type must inherit from Messager");
        auto& ptr = GetObjects()[std::type_index(typeid(T))][Id];
        if (!ptr) {
            ptr = std::make_unique<T>();
            ptr->init();
        }
        return static_cast<T&>(*ptr);
    }

    template <class T, class... ArgTs>
    static std::unique_ptr<T> New(ArgTs&&... args) {
        static_assert(std::is_base_of<Messages::Messager, T>::value,
                      "New(): Object type must inherit from Messager");
        auto ptr = std::make_unique<T>(std::forward<ArgTs>(args)...);
        ((Messages::Messager*)ptr.get())->init();
        return ptr;
    }

   private:
    // For ComponentManagers and Services
    typedef std::unordered_map<std::type_index, Messages::MessagerPtr>
        SingletonMap;
    static SingletonMap& GetSingletons();

    // For Entities
    typedef std::unordered_map<std::type_index,
                               std::unordered_map<int, Messages::MessagerPtr>>
        ObjectMap;
    static ObjectMap& GetObjects();
};

#endif
