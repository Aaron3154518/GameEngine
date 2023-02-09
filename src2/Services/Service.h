#ifndef SERVICE_H
#define SERVICE_H

#include <Components/ComponentManager.h>

namespace Services {
class Service : public Components::ComponentManager<Components::Component> {
   protected:
    struct iterable {
       public:
        typedef std::function<bool(const Entities::UUID&)> CheckFunc;

        class iterator {
           public:
            iterator(const iterator_base& it, const iterator_base& end,
                     const CheckFunc& func);

            bool operator==(const iterator& rhs) const;
            bool operator!=(const iterator& rhs) const;

            Components::EntityComponents operator*();

            // Prefix ++
            iterator& operator++();

           private:
            iterator_base mIt, mEnd;
            CheckFunc mCheck;
        };

        iterable(const iterator_base& bIt, const iterator_base& eIt,
                 const CheckFunc& check);

        iterator begin();
        iterator end();

       private:
        iterator mBegin, mEnd;
    };

    template <class... Ts>
    typename std::enable_if<sizeof...(Ts) == 0, bool>::type check(
        const Entities::UUID& eId) {
        return true;
    }

    template <class T, class... Ts>
    typename std::enable_if<sizeof(T) + sizeof...(Ts) != 0, bool>::type check(
        const Entities::UUID& eId) {
        return Components::Get<T>(eId).isActive() && check<Ts...>(eId);
    }

    template <class... CompManTs>
    iterable active() {
        return iterable(iterator_base(mComponents.begin()),
                        iterator_base(mComponents.end()),
                        [this](const Entities::UUID& eId) {
                            return check<CompManTs...>(eId);
                        });
    }
};
}  // namespace Services

#endif
