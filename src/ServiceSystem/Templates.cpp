#include "Templates.h"

// MyModel
class MyModel : public Model<TypeWrapper<>, TypeWrapper<int, bool>,
                             bool(std::string, int), void(), void(), int> {
   public:
    MyModel() {}

    // void next() { std::cerr << "Custom Next" << std::endl; }

    void next(int i, bool b) {
        std::cerr << "Spicy Next: " << i << " " << b << std::endl;
    }
};

void testModel() {
    MyModel m;

    next<0>(m);
    next<1>(m, 2452, true);

    set<0>(m, std::make_shared<int>(69));
    std::cerr << get<0>(m).get() << " " << *get<0>(m) << std::endl;

    setFunc<0>(m) = [](std::string str, bool b) -> bool {
        std::cerr << str << ": " << b << std::endl;
        return !b;
    };
    setFunc<1>(m) = []() -> void { std::cerr << "Hello"; };
    setFunc<2>(m) = []() -> void { std::cerr << " World" << std::endl; };
    std::cerr << call<0>(m, std::string("Sup"), 9) << std::endl;
    call<1>(m);
    call<2>(m);
}

// MyObservable
class MyObservable
    : public Observable<void(), void(int, bool),
                        Subscription<void(), int(bool), void(bool)>> {
   public:
    void next() {
        for (auto sub : mSubscriptions) {
            call<0>(*sub);
        }
    }

    void next(int i, bool b) {
        for (auto sub : mSubscriptions) {
            call<1>(*sub, i > 0);
            call<2>(*sub, i > 0 && b);
        }
    }
};

class SimpleObservable : public ForwardObservable<int> {};

void testObservable() {
    MyObservable m;

    MyObservable::SubscriptionPtr mSub = m.subscribe(
        []() { std::cerr << "void()" << std::endl; },
        [](bool b) {
            std::cerr << "bool(" << b << ")" << std::endl;
            return 200;
        },
        [](bool b) { std::cerr << "void(" << b << ")" << std::endl; });

    m.next();
    m.next(200, false);
    m.next(-1, true);

    SimpleObservable s;
    SimpleObservable::SubscriptionPtr mSub2 = s.subscribe(
        [](int i) { std::cerr << "Too simple " << i << std::endl; });

    s.next(6);
    s.next(-9);
}

// Main
int main(int argc, char* argv[]) { testObservable(); }