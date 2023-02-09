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
                     const CheckFunc& func)
                : mIt(it), mEnd(end), mCheck(func) {
                while (mIt != mEnd && !mCheck(mIt.id())) {
                    ++mIt;
                }
            }

            bool operator==(const iterator& rhs) const {
                return mIt == rhs.mIt;
            }
            bool operator!=(const iterator& rhs) const {
                return !(*this == rhs);
            }

            Components::EntityComponents operator*() {
                return Components::EntityComponents(mIt.id());
            }

            // Prefix ++
            iterator& operator++() {
                do {
                    ++mIt;
                } while (mIt != mEnd && !mCheck(mIt.id()));
                return *this;
            }

           private:
            iterator_base mIt, mEnd;
            CheckFunc mCheck;
        };

        iterable(const iterator_base& bIt, const iterator_base& eIt,
                 const CheckFunc& check)
            : mBegin(bIt, eIt, check), mEnd(eIt, eIt, check) {}

        iterator begin() { return mBegin; }
        iterator end() { return mEnd; }

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
    iterable require() {
        return iterable(iterator_base(mComponents.begin()),
                        iterator_base(mComponents.end()),
                        [this](const Entities::UUID& eId) {
                            return check<CompManTs...>(eId);
                        });
    }
};
}  // namespace Services

#endif
