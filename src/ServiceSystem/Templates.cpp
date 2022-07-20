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

// Main
int main(int argc, char* argv[]) {
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