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
    DataComponent(const Observables::RootNodeBase<T>& node) { setSource(node); }
    DataComponent(const Observables::StemNodeBase<T>& node) { setSource(node); }

    void setSource(const Observables::RootNodeBase<T>& node) {
        mGet = node.getter();
        mSet = node.setter();
    }
    void setSource(const Observables::StemNodeBase<T>& node) {
        mGet = node.getter();
        mSet = [](const T& t) {
            throw std::runtime_error(
                "DataComponent::setSource(): Cannot set value for stem node");
        };
    }
    void setSource(const T& t) {
        mGet = [=]() -> const T& { return t; };
        mSet = [this](const T& t) { mGet = [=]() -> const T& { return t; }; };
    }

    void set(const T& t) { mSet(t); }
    const T& get() const { return mGet(); }

   protected:
    std::function<const T&()> mGet;
    std::function<void(const T&)> mSet;
};

template <class T>
class DataPtrComponent : public Component {
   public:
    using Data = T;
    using DataPtr = std::unique_ptr<T>;

    DataPtrComponent() = default;
    DataPtrComponent(DataPtr tPtr) { set(std::move(tPtr)); }

    void set(DataPtr tPtr) { mTPtr = std::move(tPtr); }
    T& get() {
        if (!mTPtr) {
            throw std::runtime_error(
                "DataPtrComponent::get(): Pointer is null");
        }
        return *mTPtr;
    }

   protected:
    DataPtr mTPtr;
};
}  // namespace Components

#endif