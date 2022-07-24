#include "Templates.h"

// MyObservable
class MyObservable : public Observable<void(), int(bool), void(bool)> {
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

typedef ForwardObservable<void(int), void(int, int)> SimpleObservableBase;
class SimpleObservable : public SimpleObservableBase {
   public:
    using SimpleObservableBase::next;

    void next(int i) {
        for (auto sub : mSubscriptions) {
            call<1>(*sub, i * 2, i * i);
        }
    }
};

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

    int c = 10;

    SimpleObservable s;
    SimpleObservable::SubscriptionPtr mSub2 = s.subscribe(
        [c](int i) { std::cerr << "To Simple " << c << " " << i << std::endl; },
        [&c](int i, int j) {
            std::cerr << "Too simple " << c << " " << i << " " << j
                      << std::endl;
        });

    s.next(6, 9);
    s.next(-42);
}

template <class>
struct Print;

template <size_t... Is>
struct Print<Range<Is...>> {
    static void print() { (void)(int[]){(std::cerr << Is << " ", 0)...}; }
};

typedef RangeGen<0, 1024 * 1024> test;

// Main
int main(int argc, char* argv[]) {
    testObservable();

    Print<RangeGen<0, 70>>::print();
}