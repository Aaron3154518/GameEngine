#ifndef NUMBER_h
#define NUMBER_h

#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

class Number {
   public:
    Number() = default;
    Number(int layer, float exp, int sign);  // From tetration notation
    Number(float val);                       // From a float
    Number(float val, int exp);              // From scientific notation
    Number(std::string str);                 // From string
    ~Number() = default;

    float toFloat() const;

    Number copy() const;
    void copy(const Number &n);

    void balance();
    bool isZero() const { return mSign == 0; }

    // Math operators
    // -n
    Number &negate() {
        mSign *= -1;
        return *this;
    }
    Number operator-() const { return copy().negate(); }
    // n + m
    Number &add(const Number &num);
    Number &operator+=(const Number &rhs) {
        add(rhs);
        return *this;
    }
    // n - m
    Number &subtract(const Number &num);
    Number &operator-=(const Number &rhs) {
        subtract(rhs);
        return *this;
    }
    // n * m
    Number &multiply(const Number &num);
    Number &operator*=(const Number &rhs) {
        multiply(rhs);
        return *this;
    }
    // n / m
    Number &divide(const Number &num);
    Number &operator/=(const Number &rhs) {
        divide(rhs);
        return *this;
    }
    // n ^ m
    Number &power(const Number &num);
    Number &operator^=(const Number &rhs) {
        power(rhs);
        return *this;
    }

    // Math functions - foo() writes the result to and returns the caller
    //                - fooCopy() writes the result to and returns a copy of the
    //                caller
    // returns x given n = 10^x (e.g. 10^10^10^9 would return 10^10^9)
    Number &getExponent();
    // 1/n
    Number &getReciprocal();
    // |n|
    Number &absVal() {
        mSign *= mSign;
        return *this;
    }
    // 10^n
    Number &powTen();
    // log(n)
    Number &logTen();
    // logb(n)
    Number &logBase(const Number &base);
    // n^.5
    Number &sqrt();
    // Floor and ceiling functions
    Number &floorNum();
    Number &ceilNum();

    Number getExponentCopy() const { return copy().getExponent(); }
    Number getReciprocalCopy() const { return copy().getReciprocal(); }
    Number absValCopy() const { return copy().absVal(); }
    Number powTenCopy() const { return copy().powTen(); }
    Number logTenCopy() const { return copy().logTen(); }
    Number logBaseCopy(const Number &base) const {
        return copy().logBase(base);
    }
    Number floorCopy() const { return copy().floorNum(); }
    Number ceilCopy() const { return copy().ceilNum(); }
    Number sqrtCopy() const { return copy().sqrt(); }

    // Comparison Functions
    bool equal(const Number &other) const;
    bool less(const Number &other) const;

    void printAll() const;
    std::string toString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const Number &rhs);

   private:
    // Default value = 0
    float mExp = 0.;
    int mLayer = 0, mSign = 0;
};

// Binary operators
static Number operator+(const Number &lhs, const Number &rhs) {
    return lhs.copy() += rhs;
}
static Number operator-(const Number &lhs, const Number &rhs) {
    return lhs.copy() -= rhs;
}
static Number operator*(const Number &lhs, const Number &rhs) {
    return lhs.copy() *= rhs;
}
static Number operator/(const Number &lhs, const Number &rhs) {
    return lhs.copy() /= rhs;
}
static Number operator^(const Number &lhs, const Number &rhs) {
    return lhs.copy() ^= rhs;
}
static bool operator==(const Number &lhs, const Number &rhs) {
    return lhs.equal(rhs);
}
static bool operator!=(const Number &lhs, const Number &rhs) {
    return !lhs.equal(rhs);
}
static bool operator<(const Number &lhs, const Number &rhs) {
    return lhs.less(rhs);
}
static bool operator<=(const Number &lhs, const Number &rhs) {
    return !rhs.less(lhs);
}
static bool operator>(const Number &lhs, const Number &rhs) {
    return rhs.less(lhs);
}
static bool operator>=(const Number &lhs, const Number &rhs) {
    return !lhs.less(rhs);
}

static float round(float val, float precision);
static Number min(const Number &a, const Number &b) { return a <= b ? a : b; }
static Number max(const Number &a, const Number &b) { return a >= b ? a : b; }
/*// Math functions - these write to a copy of the number and return the copy
static Number getExponent(const Number& num) { return num.copy().getExponent();
} static Number getReciprocal(const Number& num) { return
num.copy().getReciprocal(); } static Number absVal(const Number& num) { return
num.copy().absVal(); } static Number powTen(const Number& num) { return
num.copy().powTen(); } static Number logTen(const Number& num) { return
num.copy().logTen(); } static Number logBase(const Number& num, const Number&
base) { return num.copy().logBase(base); } static Number floorNum(const Number&
num) { return num.copy().floorNum(); } static Number ceilNum(const Number& num)
{ return num.copy().ceilNum(); }*/

#endif
