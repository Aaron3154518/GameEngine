#include "Templates.h"

// Unsubscriber
void Unsubscriber::unsubscribe() { *mSubscribed = false; }

Unsubscriber::operator bool() const { return *mSubscribed; }

// MyObservable
class MyObservable
    : public Observable<void(), int(bool), std::string, void(bool)> {
   public:
    void next() {
        for (auto sub : *this) {
            std::cerr << get<0>(*sub) << std::endl;
            call<0>(*sub);
        }
    }

    void next(int i, bool b) {
        for (auto it = begin(), itEnd = end(); it != itEnd; ++it) {
            call<1>(**it, i > 0);
            call<2>(**it, i > 0 && b);
        }
    }
};

typedef ForwardObservable<void(int), void(int, int)> SimpleObservableBase;
class SimpleObservable : public SimpleObservableBase {
   public:
    using SimpleObservableBase::next;

    void next(int i) {
        for (auto sub : *this) {
            call<0>(*sub, 100 + i);
            call<1>(*sub, i * 2, i * i);
        }
    }
};

void testObservable() {
    MyObservable m;

    std::string str = "Hello World";

    MyObservable::SubscriptionPtr mSub = m.subscribe(
        []() { std::cerr << "void()" << std::endl; },
        [](bool b) {
            std::cerr << "bool(" << b << ")" << std::endl;
            return 200;
        },
        [](bool b) { std::cerr << "void(" << b << ")" << std::endl; }, str);

    m.next();
    m.next(200, false);
    m.next(-1, true);

    int c = 10;

    SimpleObservable s;
    {
        SimpleObservable::SubscriptionPtr mSub2 = s.subscribe(
            [c](int i) {
                std::cerr << "To Simple " << c << " " << i << std::endl;
            },
            [&c](int i, int j) {
                std::cerr << "Too simple " << c << " " << i << " " << j
                          << std::endl;
            });

        s.next(6, 9);
    }

    std::cerr << "Reset" << std::endl;

    s.next(-42);
}

// Main
int main(int argc, char* argv[]) { testObservable(); }