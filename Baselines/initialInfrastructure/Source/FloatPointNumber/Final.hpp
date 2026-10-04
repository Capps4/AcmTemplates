#pragma once
#include <cmath>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <utility>
#include <iomanip>
#include <istream>
#include <ostream>
#include <type_traits>

// SNIPPET BEGIN
template <class T>
class FloatPointNumber {
    static_assert(std::is_floating_point_v<T>);
    static constexpr T EPS = T(1E-12);
    inline static int precision = 6;
    inline static thread_local std::string tokenCache;
    T x{};

    static constexpr int sgn(T value) { return value < -EPS ? -1 : value > EPS; }

public:
    constexpr FloatPointNumber() = default;

    constexpr FloatPointNumber(T value) : x(value) {}

    constexpr T val() const { return x; }

    constexpr operator T() const { return x; }

    constexpr int sgn() const { return sgn(x); }

    template <class G>
    G round() const {
        return G(std::round(x));
    }

    static void setprecision(int len) { precision = len; }

    constexpr FloatPointNumber &operator+=(FloatPointNumber value) & {
        x += value.x;
        return *this;
    }

    constexpr FloatPointNumber &operator-=(FloatPointNumber value) & {
        x -= value.x;
        return *this;
    }

    constexpr FloatPointNumber &operator*=(FloatPointNumber value) & {
        x *= value.x;
        return *this;
    }

    constexpr FloatPointNumber &operator/=(FloatPointNumber value) & {
        x = T(static_cast<long double>(x) / static_cast<long double>(value.x));
        return *this;
    }

    constexpr FloatPointNumber operator+() const { return *this; }

    constexpr FloatPointNumber operator-() const { return -x; }

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

    friend constexpr bool operator<(FloatPointNumber a, FloatPointNumber b) { return a.x < b.x; }

    friend constexpr bool operator>(FloatPointNumber a, FloatPointNumber b) { return a.x > b.x; }

    friend constexpr bool operator==(FloatPointNumber a, FloatPointNumber b) {
        T delta = a.x - b.x;
        return a.x == b.x || (-EPS <= delta && delta <= EPS);
    }

    friend constexpr bool operator!=(FloatPointNumber a, FloatPointNumber b) { return !(a == b); }

    friend constexpr bool operator<=(FloatPointNumber a, FloatPointNumber b) {
        return a.x < b.x || a == b;
    }

    friend constexpr bool operator>=(FloatPointNumber a, FloatPointNumber b) {
        return a.x > b.x || a == b;
    }

    // Exact scalar matches keep implicit conversion for std::sqrt/cos/etc unambiguous.
#define FLOAT_POINT_SCALAR_OP(op) \
    template <class U, std::enable_if_t<std::is_arithmetic_v<U>, int> = 0> \
    friend constexpr auto operator op(FloatPointNumber a, U b) { \
        return a op FloatPointNumber(T(b)); \
    } \
    template <class U, std::enable_if_t<std::is_arithmetic_v<U>, int> = 0> \
    friend constexpr auto operator op(U a, FloatPointNumber b) { \
        return FloatPointNumber(T(a)) op b; \
    }
    FLOAT_POINT_SCALAR_OP(+)
    FLOAT_POINT_SCALAR_OP(-)
    FLOAT_POINT_SCALAR_OP(*)
    FLOAT_POINT_SCALAR_OP(/)
    FLOAT_POINT_SCALAR_OP(<)
    FLOAT_POINT_SCALAR_OP(>)
    FLOAT_POINT_SCALAR_OP(<=)
    FLOAT_POINT_SCALAR_OP(>=)
    FLOAT_POINT_SCALAR_OP(==)
    FLOAT_POINT_SCALAR_OP(!=)
#undef FLOAT_POINT_SCALAR_OP

    template <class Input>
    friend Input &operator>>(Input &in, FloatPointNumber &a) {
        // Move the cached capacity into this call: nested reads own separate tokens.
        std::string str = std::move(tokenCache);
        if (!(in >> str)) {
            tokenCache = std::move(str);
            return in;
        }
        char *end = nullptr;
        errno = 0;
        T v;
        if constexpr (std::is_same_v<T, float>)
            v = std::strtof(str.c_str(), &end);
        else if constexpr (std::is_same_v<T, double>)
            v = std::strtod(str.c_str(), &end);
        else
            v = std::strtold(str.c_str(), &end);
        bool bad = end == str.c_str() || end != str.c_str() + str.size() ||
                   (errno == ERANGE && (v == 0 || !std::isfinite(v)));
        if (bad) {
            if constexpr (std::is_base_of_v<std::istream, Input>)
                in.setstate(std::ios_base::failbit);
            else
                in.setFail();
        } else
            a.x = v;
        tokenCache = std::move(str);
        return in;
    }

    template <class Output>
    friend Output &operator<<(Output &out, FloatPointNumber a) {
        if constexpr (std::is_base_of_v<std::ostream, Output>)
            out << std::fixed << std::setprecision(precision) << a.x;
        else
            out.writeReal(a.x, precision);
        return out;
    }

    friend constexpr FloatPointNumber abs(FloatPointNumber value) {
        return value.x < 0 ? -value : value;
    }

    friend FloatPointNumber sqrt(FloatPointNumber value) { return std::sqrt(value.x); }

    friend FloatPointNumber sin(FloatPointNumber value) { return std::sin(value.x); }

    friend FloatPointNumber cos(FloatPointNumber value) { return std::cos(value.x); }

    friend FloatPointNumber atan2(FloatPointNumber y, FloatPointNumber x) {
        return std::atan2(y.x, x.x);
    }

    static const FloatPointNumber PI;
};

template <class T>
inline const FloatPointNumber<T> FloatPointNumber<T>::PI = T(std::acos(-1.L));

using Float = FloatPointNumber<double>;
