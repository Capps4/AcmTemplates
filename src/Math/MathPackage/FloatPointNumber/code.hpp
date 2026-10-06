#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
template <class T>
class FloatPointNumber {
    static_assert(std::is_floating_point_v<T>);
    static constexpr T EPS = T(1E-12);
    inline static int prec = 10;
    inline static std::string buf;
    T x{};

    static constexpr int sgn(T v) {
        return v < -EPS ? -1 : v > EPS;
    }

public:
    constexpr FloatPointNumber() = default;

    constexpr FloatPointNumber(T v) : x(v) {}

    constexpr T val() const {
        return x;
    }

    constexpr int sgn() const {
        return sgn(x);
    }

    template <class G>
    G round() const {
        const T ip = std::floor(x);
        const T fp = x - ip;
        return G(ip + (fp >= T(0.5) - EPS));
    }

    static void setprecision(int len) {
        prec = len;
    }

    constexpr FloatPointNumber &operator+=(FloatPointNumber v) & {
        x += v.x;
        return *this;
    }

    constexpr FloatPointNumber &operator-=(FloatPointNumber v) & {
        x -= v.x;
        return *this;
    }

    constexpr FloatPointNumber &operator*=(FloatPointNumber v) & {
        x *= v.x;
        return *this;
    }

    constexpr FloatPointNumber &operator/=(FloatPointNumber v) & {
        x = T(static_cast<long double>(x) / static_cast<long double>(v.x));
        return *this;
    }

    constexpr FloatPointNumber operator+() const {
        return *this;
    }

    constexpr FloatPointNumber operator-() const {
        return -x;
    }

    friend constexpr FloatPointNumber operator+(FloatPointNumber a, FloatPointNumber b) {
        return a += b;
    }

    friend constexpr FloatPointNumber operator-(FloatPointNumber a, FloatPointNumber b) {
        return a -= b;
    }

    friend constexpr FloatPointNumber operator*(FloatPointNumber a, FloatPointNumber b) {
        return a *= b;
    }

    friend constexpr FloatPointNumber operator/(FloatPointNumber a, FloatPointNumber b) {
        return a /= b;
    }

    friend constexpr bool operator<(FloatPointNumber a, FloatPointNumber b) {
        return a.x < b.x;
    }

    friend constexpr bool operator>(FloatPointNumber a, FloatPointNumber b) {
        return a.x > b.x;
    }

    friend constexpr bool operator==(FloatPointNumber a, FloatPointNumber b) {
        T dif = a.x - b.x;
        return a.x == b.x or (-EPS <= dif and dif <= EPS);
    }

    friend constexpr bool operator!=(FloatPointNumber a, FloatPointNumber b) {
        return !(a == b);
    }

    friend constexpr bool operator<=(FloatPointNumber a, FloatPointNumber b) {
        return a.x < b.x or a == b;
    }

    friend constexpr bool operator>=(FloatPointNumber a, FloatPointNumber b) {
        return a.x > b.x or a == b;
    }

    template <class Input>
    friend Input &operator>>(Input &in, FloatPointNumber &a) {
        if (!(in >> buf))
            return in;
        if constexpr (std::is_same_v<T, float>)
            a.x = std::stof(buf);
        else if constexpr (std::is_same_v<T, double>)
            a.x = std::stod(buf);
        else
            a.x = std::stold(buf);
        return in;
    }

    template <class Output>
    friend Output &operator<<(Output &out, FloatPointNumber a) {
        out << std::fixed << std::setprecision(prec) << a.x;
        return out;
    }
};

namespace std {
#define FLOAT_POINT_UNARY_FN(fn) \
    template <class T> \
    ::FloatPointNumber<T> fn(::FloatPointNumber<T> a) { \
        return std::fn(a.val()); \
    }
FLOAT_POINT_UNARY_FN(abs)
FLOAT_POINT_UNARY_FN(sqrt)
FLOAT_POINT_UNARY_FN(sin)
FLOAT_POINT_UNARY_FN(cos)
FLOAT_POINT_UNARY_FN(tan)
FLOAT_POINT_UNARY_FN(asin)
FLOAT_POINT_UNARY_FN(acos)
FLOAT_POINT_UNARY_FN(atan)
#undef FLOAT_POINT_UNARY_FN

template <class T>
::FloatPointNumber<T> atan2(::FloatPointNumber<T> y, ::FloatPointNumber<T> x) {
    return std::atan2(y.val(), x.val());
}

template <class T>
::FloatPointNumber<T> fma(::FloatPointNumber<T> a, ::FloatPointNumber<T> b,
                          ::FloatPointNumber<T> c) {
    return std::fma(a.val(), b.val(), c.val());
}
}

using Float = FloatPointNumber<double>;
