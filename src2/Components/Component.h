#ifndef COMPONENT_H
#define COMPONENT_H

#include <Observables/Observables.h>

#include <memory>

namespace Components {
class Component {
   public:
    virtual ~Component() = default;

    void setActive(bool active);
    bool isActive() const;

   private:
    bool mActive = true;
};

typedef std::unique_ptr<Component> ComponentPtr;

template <class T>
class DataComponent : public Component {
   public:
    using Data = T;

    DataComponent(const T& t) { setSource(t); }
    virtual ~DataComponent() = default;

    template <class CodeT, class IdT>
    void setSource(Observables::Node<T, CodeT, IdT> node) {
        mGet = [=]() { return node(); };
        mSet = [=](const T& t) { node(t); };
    }
    void setSource(const T& t) {
        mGet = [=]() { return t; };
        mSet = [this](const T& t) { mGet = [=]() { return t; } }
    }

    void set(const T& t) { mSet(t); }
    const T& get() const { return mGet(); }

   protected:
    std::function<const T&()> mGet;
    std::function<void(const T&)> mSet;
};
}  // namespace Components

#endif