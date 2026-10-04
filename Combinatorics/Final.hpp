#pragma once
#include <algorithm>
#include <cassert>
#include <type_traits>
#include <vector>
#include "../ModuloInteger/Final.hpp"

// SNIPPET BEGIN
// Requires a prime modulus and factorial indices smaller than the modulus.
template <class T>
class Comb {
    int n = 0;
    std::vector<T> factorial{T(1)}, inverseFactorial{T(1)};
    decltype(T::getMod()) modulus = T::getMod();

    void checkMod() {
        if (modulus != T::getMod()) {
            modulus = T::getMod();
            n = 0;
            factorial.assign(1, T(1));
            inverseFactorial.assign(1, T(1));
        }
    }

    void checkSize(int x) {
        assert(x >= 0 && x < T::getMod());
        checkMod();
        if (x > n) init(int(std::min<long long>(2LL * x, modulus - 1)));
    }

public:
    explicit Comb(int m = 0) { init(m); }

    static Comb &shared(int m = 8) {
        static Comb instance;
        instance.init(m);
        return instance;
    }

    void init(int m) {
        assert(m >= 0 && T::getMod() > 1);
        checkMod();
        m = int(std::min<long long>(m, modulus - 1));
        if (m <= n) return;
        factorial.resize(m + 1);
        inverseFactorial.resize(m + 1);
        for (int i = n; i < m; ++i)
            factorial[i + 1] = factorial[i] * (i + 1);
        inverseFactorial[m] = factorial[m].inv();
        for (int i = m; i > n; --i)
            inverseFactorial[i - 1] = inverseFactorial[i] * i;
        n = m;
    }

    T jc(int x) {
        checkSize(x);
        return factorial[x];
    }

    T ijc(int x) {
        checkSize(x);
        return inverseFactorial[x];
    }

    T A(int a, int b) {
        if (a < b || b < 0) return 0;
        checkSize(a);
        return factorial[a] * inverseFactorial[a - b];
    }

    T C(int a, int b) {
        if (a < b || b < 0) return 0;
        checkSize(a);
        return factorial[a] * inverseFactorial[a - b] * inverseFactorial[b];
    }
};

inline auto &comb = Comb<Z>::shared();
