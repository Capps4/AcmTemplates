#pragma once
#include "Include.hpp"

// SNIPPET BEGIN
// Requires a prime modulus and factorial indices smaller than the modulus.
template <class T>
class Comb {
    int n = 0;
    std::vector<T> _jc{T(1)}, _ijc{T(1)};
    decltype(T::getMod()) mod = T::getMod();

    void sync() {
        if (mod != T::getMod()) {
            mod = T::getMod();
            n = 0;
            _jc.assign(1, T(1));
            _ijc.assign(1, T(1));
        }
    }

    void fit(int x) {
        assert(x >= 0 and x < T::getMod());
        sync();
        if (x > n)
            init(int(std::min<long long>(2LL * x, mod - 1)));
    }

public:
    explicit Comb(int m = 0) {
        init(m);
    }

    static Comb &shared(int m = 8) {
        static Comb obj;
        obj.init(m);
        return obj;
    }

    void init(int m) {
        assert(m >= 0 and T::getMod() > 1);
        sync();
        m = int(std::min<long long>(m, mod - 1));
        if (m <= n)
            return;
        _jc.resize(m + 1);
        _ijc.resize(m + 1);
        for (int i = n; i < m; ++i)
            _jc[i + 1] = _jc[i] * (i + 1);
        _ijc[m] = _jc[m].inv();
        for (int i = m; i > n; --i)
            _ijc[i - 1] = _ijc[i] * i;
        n = m;
    }

    T jc(int x) {
        fit(x);
        return _jc[x];
    }

    T ijc(int x) {
        fit(x);
        return _ijc[x];
    }

    T A(int a, int b) {
        if (a < b or b < 0)
            return 0;
        fit(a);
        return _jc[a] * _ijc[a - b];
    }

    T C(int a, int b) {
        if (a < b or b < 0)
            return 0;
        fit(a);
        return _jc[a] * _ijc[a - b] * _ijc[b];
    }
};

inline auto &comb = Comb<Z>::shared();
