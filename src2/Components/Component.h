#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>

namespace Components {
class Component {
   public:
    virtual ~Component() = default;
};

typedef std::unique_ptr<Component> ComponentPtr;

template <class T>
class DataComponent : public Component {
   public:
    DataComponent(const T& t) : mT(t) {}
    virtual ~DataComponent() = default;

    void set(const T& t) { mT = t; }
    T& get() { return mT; }
    const T& get() const { return mT; }

   private:
    T mT;
};
}  // namespace Components

#endif