#ifndef COMPONENT_H
#define COMPONENT_H

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

class DataComponentBase {
   public:
    ~DataComponentBase() = default;
};

template <class T>
class DataComponent : public Component, public DataComponentBase {
   public:
    using Data = T;

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