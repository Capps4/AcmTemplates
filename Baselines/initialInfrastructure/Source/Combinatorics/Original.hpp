#pragma once
#include <bits/stdc++.h>
using i64 = long long;
template <class T>
class Comb {
    int n;
    std::vector<T> _jc, _ijc;

    void _checkSize(int x) {
        if (x > n) {
            init(x * 2);
        }
    }

    constexpr Comb()
        : n{0},
        _jc{1},
        _ijc{1} {}

    explicit Comb(int m) : Comb() {
        init(m);
    }
public:
    static Comb &shared(int m = 8) {
        static Comb instance(m);
        return instance;
    }

    void init(int m) {
        m = std::min(m, T::getMod() - 1);
        if (m <= n) {
            return;
        }

        _jc.resize(m + 1);
        _ijc.resize(m + 1);

        for (int i = n + 1; i <= m; i++) {
            _jc[i] = _jc[i - 1] * i;
        }
        _ijc.back() = _jc.back().inv();
        for (int i = m; i > n; i--) {
            _ijc[i - 1] = _ijc[i] * i;
        }

        n = m;
    }

    T jc(int x) {
        _checkSize(x);
        return _jc[x];
    }

    T ijc(int x) {
        _checkSize(x);
        return _ijc[x];
    }

    T A(int a, int b) {
        if (a < b or b < 0) {
            return 0;
        }
        _checkSize(a);
        return _jc[a] * _ijc[a - b];
    }
    T C(int a, int b) {
        if (a < b or b < 0) {
            return 0;
        }
        _checkSize(a);
        return _jc[a] * _ijc[a - b] * _ijc[b];
    }
};
auto comb = Comb<Z>::shared();

