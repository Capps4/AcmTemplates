#pragma once
#include <bits/stdc++.h>
using i64 = long long;
template <class T>
class FloatPointNumber {
    static constexpr T EPS = 1E-12;
    static_assert(EPS >= 0, "EPS < 0");

    static constexpr int sgn(T x) {
        return x < -EPS ? -1 : x > EPS;
    }

    static int precision;
    static std::string inputStr;

    T x;
public:
    constexpr FloatPointNumber() : x{} {}
    constexpr FloatPointNumber(T x) : x{x} {}

    constexpr T val() const {
        return x;
    }

    constexpr int sgn() const {
        return sgn(x);
    }

    template <class G>
    constexpr G round() const {
        return G(x + 0.5 + EPS);
    }

    static void setprecision(int len) {
        precision = len;
    }

    // 四则运算

    FloatPointNumber &operator+=(FloatPointNumber a) & {
        x += a.x;
        return *this;
    }

    friend constexpr FloatPointNumber operator+(FloatPointNumber a, FloatPointNumber b) {
        return a += b;
    }

    constexpr FloatPointNumber operator-() const {
        return FloatPointNumber(-x);
    }

    FloatPointNumber &operator-=(FloatPointNumber a) & {
        x = x - a.x;
        return *this;
    }

    friend constexpr FloatPointNumber operator-(FloatPointNumber a, FloatPointNumber b) {
        return a -= b;
    }

    FloatPointNumber &operator*=(FloatPointNumber a) & {
        x *= a.x;
        return *this;
    }

    friend constexpr FloatPointNumber operator*(FloatPointNumber a, FloatPointNumber b) {
        return a *= b;
    }

    FloatPointNumber &operator/=(FloatPointNumber a) & {
        x /= (long double)a.x;
        return *this;
    }

    friend constexpr FloatPointNumber operator/(FloatPointNumber a, FloatPointNumber b) {
        return a /= b;
    }

    // 比较运算

    friend constexpr int operator<(FloatPointNumber a, FloatPointNumber b) {
        return a.x < b.x;
    }

    friend constexpr int operator>(FloatPointNumber a, FloatPointNumber b) {
        return a.x > b.x;
    }

    friend constexpr int operator<=(FloatPointNumber a, FloatPointNumber b) {
        return sgn(a.x - b.x) <= 0;
    }

    friend constexpr int operator>=(FloatPointNumber a, FloatPointNumber b) {
        return sgn(a.x - b.x) >= 0;
    }

    friend constexpr bool operator==(FloatPointNumber a, FloatPointNumber b) {
        return sgn(a.x - b.x) == 0;
    }

    friend constexpr bool operator!=(FloatPointNumber a, FloatPointNumber b) {
        return sgn(a.x - b.x) != 0;
    }

    // 输入输出
    template <class Input>
    friend Input &operator>>(Input &is, FloatPointNumber &a) {
        is >> inputStr;
        if (std::is_same<T, long double>::value) {
            a = FloatPointNumber(std::stold(inputStr));
        } else {
            a = FloatPointNumber(std::stod(inputStr));
        }
        return is;
    }

    template <class Output>
    friend Output &operator<<(Output &os, FloatPointNumber a) {
        return os << std::fixed << std::setprecision(precision) << a.val();
    }

    // 常数
    static const FloatPointNumber PI;
};

template <class T>
const FloatPointNumber<T> FloatPointNumber<T>::PI =
    FloatPointNumber<T>(acosl(-1));

template <class T>
std::string FloatPointNumber<T>::inputStr;

template <class T>
constexpr FloatPointNumber<T> std::abs(FloatPointNumber<T> x) {
    return x < 0 ? -x : x;
}

template <class T>
constexpr FloatPointNumber<T> std::atan2(FloatPointNumber<T> x, FloatPointNumber<T> y) {
    return std::atan2(1.L * x.val(), 1.L * y.val());
}

template <class T>
constexpr FloatPointNumber<T> std::sin(FloatPointNumber<T> x) {
    return std::sin(1.L * x.val());
}

template <class T>
constexpr FloatPointNumber<T> std::cos(FloatPointNumber<T> x) {
    return std::cos(1.L * x.val());
}

template <class T>
constexpr FloatPointNumber<T> std::sqrt(FloatPointNumber<T> x) {
    return std::sqrt(1.L * x.val());
}

template <class T>
int FloatPointNumber<T>::precision = 6;

using Float = FloatPointNumber<double>;

