#include <ServiceSystem/Observable.h>

struct Test {
    Test(bool _b) : b(_b) {}
    Test(const Test& rhs) {
        std::cerr << "Copied" << std::endl;
        b = rhs.b;
    }
    Test& operator=(const Test& rhs) {
        b = rhs.b;
        return *this;
    }
    ~Test() = default;

    bool b;
};

// MyObservable
class MyObservable
    : public Observable<void(), int(const Test&), std::string, void(bool)> {
   public:
    void next() {
        for (auto sub : *this) {
            std::cerr << sub->get<2>() << std::endl;
            sub->get<0>()();
        }
    }

    void next(int i, bool b) {
        for (auto it = rbegin(), itEnd = rend(); it != itEnd; ++it) {
            Test t(i > 0);
            (*it)->get<1>()(t);
            (*it)->get<3>()(i > 0 && b);
        }
    }
};

// SimpleObservable
typedef ForwardObservable<void(int), void(int, int)> SimpleObservableBase;
class SimpleObservable : public SimpleObservableBase {
   public:
    using SimpleObservableBase::next;

    void next(int&& i) {
        for (auto sub : *this) {
            sub->get<0>()(100 + i);
            sub->get<1>()(i * 2, i * i);
        }
    }
};

void testObservable() {
    MyObservable m;

    std::string str = "Hello World";

    MyObservable::SubscriptionPtr sub1 = m.subscribe(
        []() { std::cerr << "1 void()" << std::endl; },
        [](const Test& t) {
            std::cerr << "1 bool(" << t.b << ")" << std::endl;
            return 200;
        },
        str, [](bool b) { std::cerr << "1 void(" << b << ")" << std::endl; });
    MyObservable::SubscriptionPtr sub2 = m.subscribe(
        []() { std::cerr << "2 void()" << std::endl; },
        [](const Test& t) {
            std::cerr << "2 bool(" << t.b << ")" << std::endl;
            return 200;
        },
        str, [](bool b) { std::cerr << "2 void(" << b << ")" << std::endl; });
    sub2->get<2>() += " Other";

    m.next();
    std::cerr << "Order should now be 2, 1" << std::endl;
    m.next(200, false);
    m.next(-1, true);
}

void testForward() {
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

// UObservable
class UObservable : public Observable<void(), std::string> {
   public:
    void next() {
        for (auto sub : *this) {
            std::cerr << sub->get<1>() << ": ";
            sub->get<0>()();
        }
    }
};

void testUnsubscribe() {
    UObservable u;

    const int N = 4;
    int ctrs[N] = {3, 2, 1, 0};

    UObservable::SubscriptionPtr sub1 = u.subscribe(
        [&ctrs]() {
            std::cerr << "Should print 3 times" << std::endl;
            ctrs[0]--;
        },
        "Control");
    UObservable::SubscriptionPtr sub2 = u.subscribe(
        [&ctrs]() {
            std::cerr << "Should print 2 times" << std::endl;
            ctrs[1]--;
        },
        "Reset Test");
    {
        UObservable::SubscriptionPtr sub3 = u.subscribe(
            [&ctrs]() {
                std::cerr << "Should print 1 time" << std::endl;
                ctrs[2]--;
            },
            "Out of Scope Test");
        u.subscribe(
            [&ctrs]() {
                std::cerr << "Should print 0 times" << std::endl;
                ctrs[3]--;
            },
            "No Save Test");
        u.next();
    }

    u.next();

    sub2.reset();

    u.next();

    bool good = true;
    for (int i = 0; i < N; i++) {
        if (ctrs[i] != 0) {
            std::cerr << "Counter at " << i << " is " << ctrs[i]
                      << " but should be 0" << std::endl;
            good = false;
        }
    }
    if (good) {
        std::cerr << "All counters are 0" << std::endl;
    }
}

// Main
int main(int argc, char* argv[]) {
    testObservable();

    std::cerr << std::endl;
    testForward();

    std::cerr << std::endl;
    testUnsubscribe();
}