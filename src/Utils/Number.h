#ifndef NUMBER_h
#define NUMBER_h

#include <Utils/Math.h>

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
    int toInt() const;

    Number copy() const;
    void copy(const Number &n);

    void balance();
    bool isZero() const;

    // Math functions
    // -n
    Number &negate();
    // n + m
    Number &add(const Number &num);
    // n - m
    Number &subtract(const Number &num);
    // n * m
    Number &multiply(const Number &num);
    // n / m
    Number &divide(const Number &num);
    // n ^ m
    Number &power(const Number &num);

    // Math functions - foo() writes the result to and returns the caller
    //                - fooCopy() writes the result to and returns a copy of the
    //                caller
    // returns x given n = 10^x (e.g. 10^10^10^9 would return 10^10^9)
    Number &getExponent();
    // 1/n
    Number &getReciprocal();
    // |n|
    Number &absVal();
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

    Number getExponentCopy() const;
    Number getReciprocalCopy() const;
    Number absValCopy() const;
    Number powTenCopy() const;
    Number logTenCopy() const;
    Number logBaseCopy(const Number &base) const;
    Number floorCopy() const;
    Number ceilCopy() const;
    Number sqrtCopy() const;

    // Comparison Functions
    bool equal(const Number &other) const;
    bool less(const Number &other) const;
    static Number min(const Number &a, const Number &b);
    static Number max(const Number &a, const Number &b);

    // Unary operators
    Number operator-() const;
    Number &operator+=(const Number &rhs);
    Number &operator-=(const Number &rhs);
    Number &operator*=(const Number &rhs);
    Number &operator/=(const Number &rhs);
    Number &operator^=(const Number &rhs);

    // Binary operators
    Number operator+(const Number &rhs) const;
    Number operator-(const Number &rhs) const;
    Number operator*(const Number &rhs) const;
    Number operator/(const Number &rhs) const;
    Number operator^(const Number &rhs) const;
    bool operator==(const Number &rhs) const;
    bool operator!=(const Number &rhs) const;
    bool operator<(const Number &rhs) const;
    bool operator<=(const Number &rhs) const;
    bool operator>(const Number &rhs) const;
    bool operator>=(const Number &rhs) const;

    // String and print functions
    void printAll() const;
    std::string toString() const;

    friend std::ostream &operator<<(std::ostream &os, const Number &rhs);

   private:
    // Default value = 0
    float mExp = 0.;
    int mLayer = 0, mSign = 0;
};

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
