#pragma once
#include <bits/stdc++.h>
using i64 = long long;
template <class T, T P>
class ModuloInteger {
    T x;
    static T Mod;

    static constexpr int mult(int a, int b, int Mod) {
        return 1LL * a * b % Mod;
    }

    static constexpr long long mult(long long a, long long b, long long p) {
        long long res = a * b - static_cast<long long>(1.L * a * b / p) * p;
        res %= p;
        if (res < 0) {
            res += p;
        }
        return res;
    }

    constexpr T norm(T x) const {
        return (x < 0 ? x + getMod() : (x >= getMod() ? x - getMod() : x));
    }

public:
    typedef T ValueType;

    constexpr ModuloInteger() : x{} {}
    constexpr ModuloInteger(long long x) : x{norm(x % getMod())} {}

    static constexpr T getMod() {
        return (P > 0 ? P : Mod);
    }

    static constexpr void setMod(T Mod_) {
        Mod = Mod_;
    }

    constexpr T val() const {
        return x;
    }

    explicit constexpr operator T() const {
        return x;
    }

    constexpr ModuloInteger operator-() const {
        return ModuloInteger(getMod() - x);
    }

    constexpr ModuloInteger power(long long m) const {
        ModuloInteger a = *this;
        ModuloInteger res = 1;

        if (m < 0) {
            a = a.inv();
            m = -m;
        }

        for (; m > 0; m /= 2) {
            if (m & 1) {
                res *= a;
            }
            a *= a;
        }
        return res;
    }

    constexpr ModuloInteger inv() const {
        assert(x != 0);
        return this->power(getMod() - 2);
    }

    ModuloInteger &operator*=(ModuloInteger rhs) & {
        x = mult(x, rhs.x, getMod());
        return *this;
    }
    ModuloInteger &operator+=(ModuloInteger rhs) & {
        x = norm(x + rhs.x);
        return *this;
    }
    ModuloInteger &operator-=(ModuloInteger rhs) & {
        x = norm(x - rhs.x);
        return *this;
    }
    ModuloInteger &operator/=(ModuloInteger rhs) & {
        return *this *= rhs.inv();
    }

    friend constexpr ModuloInteger operator+(ModuloInteger lhs, ModuloInteger rhs) {
        return lhs += rhs;
    }

    friend constexpr ModuloInteger operator-(ModuloInteger lhs, ModuloInteger rhs) {
        return lhs -= rhs;
    }

    friend constexpr ModuloInteger operator*(ModuloInteger lhs, ModuloInteger rhs) {
        return lhs *= rhs;
    }

    friend constexpr ModuloInteger operator/(ModuloInteger lhs, ModuloInteger rhs) {
        return lhs /= rhs;
    }

    template <class Input>
    friend Input &operator>>(Input &is, ModuloInteger &a) {
        long long v;
        is >> v;
        a = ModuloInteger(v);
        return is;
    }

    template <class Output>
    friend Output &operator<<(Output &os, const ModuloInteger &a) {
        return os << a.val();
    }

    friend constexpr bool operator==(ModuloInteger lhs, ModuloInteger rhs) {
        return lhs.val() == rhs.val();
    }

    friend constexpr bool operator!=(ModuloInteger lhs, ModuloInteger rhs) {
        return lhs.val() != rhs.val();
    }
};

template <>
int ModuloInteger<int, 0>::Mod = 998244353;

template <>
long long ModuloInteger<long long, 0>::Mod = 4179340454199820289;

constexpr int P = 998244353;

using Z = ModuloInteger<std::remove_cv<decltype(P)>::type, P>;

