#include "Number.h"

constexpr auto MAX_Es = 5;         // Max number of e's to display for tetration
constexpr auto TOLERANCE = 1e-10;  // Error tolerance when comparing floats
constexpr float CONVERT_UP = 1e10;  // Value at which to increase the layer
constexpr float CONVERT_DOWN =
    log10(CONVERT_UP);  // Value at which to decrease the layer
constexpr float CONVERT_NEG =
    1 / CONVERT_UP;  // Value at which to make the layer negative
const Number MAX_float = Number(std::numeric_limits<float>::max());
const Number MIN_float = Number(std::numeric_limits<float>::min());

Number::Number(int layer, float exp, int sign)
    : mLayer(layer), mExp(exp), mSign((sign > 0) - (sign < 0)) {
    balance();
}
Number::Number(float val) : Number(0, std::abs(val), (val > 0.) - (val < 0.)) {}
Number::Number(float val, int exp)
    : Number(1, val == 0 ? 0 : log10(std::abs(val)) + exp,
             (val > 0) - (val < 0)) {}
Number::Number(std::string str) {
    Number n(0, 1, 1);
    float num1 = 0., num2 = 0.;
    int exp1 = -1, exp2 = -1;
    for (auto it = str.crbegin(); it != str.crend(); ++it) {
        if (*it == 'e' || *it == 'E') {
            n = Number(num1, exp1) ^ n;
            num1 = num2 = 0.;
            exp1 = exp2 = 0.;
        } else if (*it == '.') {
            exp1 -= exp2 + 1;
            exp2 = -1;
        } else if (*it - '0' < 10) {
            int i = *it - '0';
            if (i == 0) {
                num2 /= 10.;
                ++exp2;
            } else {
                num1 = num2 = (num2 / 10.) + i;
                exp1 = ++exp2;
            }
        }
    }
    copy(Number(num1, exp1) ^ n);
}

float Number::toFloat() const {
    if (absValCopy() >= MAX_float) {
        return mSign * std::numeric_limits<float>::max();
    }
    if (absValCopy() <= MIN_float) {
        return mSign * std::numeric_limits<float>::min();
    }
    if (mLayer == 0) {
        return mSign * mExp;
    }
    float val = mExp;
    for (int i = 1; i < abs(mLayer); ++i) {
        val = pow(10, val);
    }
    if (mLayer > 0) {
        return mSign * pow(10, val);
    }
    return mSign * pow(10, -val);
}

Number Number::copy() const { return Number(mLayer, mExp, mSign); }
void Number::copy(const Number &n) {
    mLayer = n.mLayer;
    mExp = n.mExp;
    mSign = n.mSign;
    balance();
}

void Number::balance() {
    //    std::cout << mLayer << ", " << mExp << ", " << mSign << " -> ";
    if (mSign == 0 || (mLayer == 0 & mExp == 0)) {
        mSign = 0;
        mLayer = 0;
        mExp = 0.;
        return;
    }
    if (mExp < 0.) {
        switch (mLayer) {
            case 0:
                mExp = -mExp;
                mSign = -mSign;
                break;
            case 1:
                mExp = -mExp;
                mLayer = -1;
                break;
            case -1:
                mExp = -mExp;
                mLayer = 1;
                break;
            default:
                mExp = pow(10, mExp);
                if (mLayer > 0) {
                    --mLayer;
                } else {
                    ++mLayer;
                }
                break;
        }
    }
    if (mExp < CONVERT_DOWN) {
        switch (mLayer) {
            case 0:
                if (mExp <= CONVERT_NEG) {
                    mExp = -log10(mExp);
                    mLayer = -1;
                }
                break;
            case -1:
                mExp = pow(10, -mExp);
                mLayer = 0;
                break;
            default:
                mExp = pow(10, mExp);
                if (mLayer > 0) {
                    --mLayer;
                } else {
                    ++mLayer;
                }
                break;
        }
    }
    if (mExp >= CONVERT_UP) {
        mExp = log10(mExp);
        if (mLayer >= 0) {
            ++mLayer;
        } else {
            --mLayer;
        }
    }
    //    std::cout << mLayer << ", " << mExp << ", " << mSign << " = ";
    //    std::cout << *this << std::endl;
}

Number &Number::add(const Number &num) {
    if (num.mSign == 0) {
        return *this;
    }
    if (mSign == 0) {
        copy(num);
    }
    // If either number has a layer above 1, addition is useless
    else if (std::abs(mLayer) > 1 || std::abs(num.mLayer) > 1) {
        if (less(num)) {
            copy(num);
        }
    } else {
        float x = mLayer == 0 ? log10(mExp) : mLayer * mExp;
        float y = num.mLayer == 0 ? log10(num.mExp) : num.mLayer * num.mExp;
        // 10^x + 10^y = 10^(log(10^(y-x) + 1) + x)
        // -10^x + -10^y = -(10^x + 10^y)
        if (mSign * num.mSign == 1) {
            mExp = log10(pow(10, y - x) + 1) + x;
        }
        // 10^x + -10^y = 10^(log(1 - 10^(y-x)) + x)
        // -10^x + 10^y = -(10^x + -10^y)
        else {
            // Note that y<x must be true so if y>x, swap x and y and negate
            // answer and if y=x, the answer is zero
            if (y == x) {
                mSign = 0;
            } else {
                if (y > x) {
                    float tmp = x;
                    x = y;
                    y = tmp;
                    mSign *= -1;
                }
                mExp = log10(1 - pow(10, y - x)) + x;
            }
        }
        mLayer = 1;
        balance();
    }
    return *this;
}
Number &Number::subtract(const Number &num) {
    add(-num);
    return *this;
}
Number &Number::multiply(const Number &num) {
    if (mSign == 0 || num.mSign == 0) {
        mSign = 0;
        balance();
    }
    // If either number has a layer above 2, multiplication is useless
    else if (std::abs(mLayer) > 2 || std::abs(num.mLayer) > 2) {
        if (less(num)) {
            copy(num);
        }
        mSign *= num.mSign;
    } else {
        int sign = mSign * num.mSign;
        getExponent().add(num.getExponentCopy()).powTen();
        mSign = sign;
    }
    return *this;
}
Number &Number::divide(const Number &num) {
    if (mSign == 0) {
        return *this;
    }
    if (num.mSign == 0) {
        throw std::domain_error("Divide by 0 error");
    }
    // If either number has a layer above 2, division is useless
    if (std::abs(mLayer) > 2 || std::abs(num.mLayer) > 2) {
        if (less(num)) {
            mSign = 0;
        }
        mSign *= num.mSign;
        balance();
    } else {
        int sign = mSign * num.mSign;
        getExponent().subtract(num.getExponentCopy()).powTen();
        mSign = sign;
    }
    return *this;
}
Number &Number::power(const Number &pow) {
    if (mSign == 0) {
        if (pow.mSign != 1) {
            throw std::domain_error("0^0 or 0^-x: Divide by zero error");
        }
        return *this;
    }
    if (pow.mSign == 0) {
        mLayer = 0;
        mExp = 1.;
        mSign = 1;
        return *this;
    }

    int sign = mSign;
    getExponent().multiply(pow).powTen();
    // If our base is negative, we only want the real component of the answer
    // If the power's layer is greater than 0, rounding will cause pow to
    //     be a multiple of 10 and thus pow*pi will be a multiple of 2pi
    // If the layer is less than 0, pow * pi will essentially be 0*2pi
    if (sign == -1 && pow.mLayer == 0) {
        multiply(Number(cos(pow.mExp * M_PI)));
    }
    return *this;
}

// Returns the numbers exponent (e.g. 10^10^x would return 10^x)
Number &Number::getExponent() {
    if (mSign == 0) {
        return *this;
    }
    if (mLayer == 0) {
        mExp = log10(mExp);
        mSign = 1;
    } else {
        mSign = (mLayer > 0) - (mLayer < 0);
        mLayer = std::abs(mLayer) - 1;
    }
    balance();
    return *this;
}

// Returns 1/number
Number &Number::getReciprocal() {
    if (mSign == 0) {
        throw std::domain_error("Divide by zero error");
    }
    if (mLayer == 0) {
        mExp = 1 / mExp;
    } else {
        mLayer *= -1;
    }
    balance();
    return *this;
}

// Returns 10^n
Number &Number::powTen() {
    if (mSign == 0 || mLayer < 0) {
        mSign = mExp = 1;
        mLayer = 0;
    } else {
        mLayer = mSign * (++mLayer);
        mSign = 1;
    }
    balance();
    return *this;
}

// Returns log(n)
Number &Number::logTen() {
    if (mSign != 1) {
        throw std::domain_error("Cannot take log of negative number or 0");
    }
    return getExponent();
}

// Returns logb(n)
Number &Number::logBase(const Number &base) {
    logTen().divide(base.logTenCopy());
    balance();
    return *this;
}

// Returns n^.5
Number &Number::sqrt() { return power(.5); }

// Floor and ceiling functions
Number &Number::floorNum() {
    if (mLayer < 0) {
        mSign = 0;
    } else if (mLayer == 0) {
        mExp = floor(mExp);
    }
    balance();
    return *this;
}
Number &Number::ceilNum() {
    if (mLayer < 0) {
        mLayer = 1;
        mExp = 1;
        mSign = 1;
    } else if (mLayer == 0) {
        mExp = ceil(mExp);
    }
    balance();
    return *this;
}

// Comparison Operators
bool Number::equal(const Number &num) const {
    return mLayer == num.mLayer && std::abs(mExp - num.mExp) < TOLERANCE &&
           mSign == num.mSign;
}
bool Number::less(const Number &num) const {
    if (equal(num)) {
        return false;
    }
    if (mSign * num.mSign != 1) {
        return mSign < num.mSign;
    }
    if (mLayer != num.mLayer) {
        return mLayer < num.mLayer;
    }
    // At this point the layers and signs are equal
    return (mExp < num.mExp) == (mSign == 1);
}

void Number::printAll() const {
    std::cout << "Layer = " << mLayer << ", Exponent = " << mExp
              << ", Sign = " << mSign << std::endl;
}

float round(float val, float precision) {
    return floor(val / precision + 0.5) * precision;
}

std::ostream &operator<<(std::ostream &os, const Number &rhs) {
    if (rhs.mSign == -1) {
        os << "-";
    }
    if (rhs.mLayer < 0) {
        os << "1/";
    }
    if (std::abs(rhs.mLayer) == 1) {
        int exp = (int)rhs.mExp;
        float m = floor(pow(10, rhs.mExp - exp) * 100.) / 100.;
        os << m << "e" << exp;
    } else {
        if (std::abs(rhs.mLayer) < MAX_Es) {
            for (int i = 0; i < std::abs(rhs.mLayer); ++i) {
                os << "e";
            }
        } else {
            os << "10^^" << std::abs(rhs.mLayer) << "^";
        }
        if (rhs.mExp == 0. || 0.01 <= rhs.mExp && rhs.mExp < 1000) {
            os << round(rhs.mExp, 0.01);
        } else {
            float val = log10(rhs.mExp);
            os << round(pow(10, val - (int)val), 0.01) << "e" << (int)val;
        }
    }
    return os;
}
