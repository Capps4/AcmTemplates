#pragma once
#include <bits/stdc++.h>
using i64 = long long;
template <class T>
struct Polynomial : public std::vector<T> {
    #define self (*this)
    static std::vector<T> w;
    static constexpr auto P = T::getMod();

    static void initW(int r) {
        if (static_cast<int>(w.size()) >= r) {
            return;
        }

        w.assign(r, 0);
        w[r >> 1] = 1;
        T s = T(3).power((P - 1) / r);
        for (int i = r / 2 + 1; i < r; i++) {
            w[i] = w[i - 1] * s;
        }
        for (int i = r / 2 - 1; i > 0; i--) {
            w[i] = w[i * 2];
        }
    }

    friend void dft(Polynomial &a) {
        const int n = a.size();
        assert((n & (n - 1)) == 0);
        initW(n);

        for (int k = n >> 1; k; k >>= 1) {
            for (int i = 0; i < n; i += k << 1) {
                for (int j = 0; j < k; j++) {
                    T v = a[i + j + k];
                    a[i + j + k] = (a[i + j] - v) * w[k + j];
                    a[i + j] = a[i + j] + v;
                }
            }
        }
    }

    friend void idft(Polynomial &a) {
        const int n = a.size();
        assert((n & (n - 1)) == 0);
        initW(n);

        for (int k = 1; k < n; k <<= 1) {
            for (int i = 0; i < n; i += k << 1) {
                for (int j = 0; j < k; j++) {
                    T x = a[i + j];
                    T y = a[i + j + k] * w[j + k];
                    a[i + j + k] = x - y;
                    a[i + j] = x + y;
                }
            }
        }

        a *= P - (P - 1) / n;
        std::reverse(a.begin() + 1, a.end());
    }

public:
    using std::vector<T>::vector;

    Polynomial mod(int k) const {
        Polynomial p = self;
        p.resize(k);
        return p;
    }

    friend Polynomial operator+(const Polynomial &a, const Polynomial &b) {
        Polynomial p(std::max(a.size(), b.size()));
        for (auto i = 0U; i < a.size(); i++) {
            p[i] += a[i];
        }
        for (auto i = 0U; i < b.size(); i++) {
            p[i] += b[i];
        }
        return p;
    }

    friend Polynomial operator-(const Polynomial &a, const Polynomial &b) {
        Polynomial p(std::max(a.size(), b.size()));
        for (auto i = 0U; i < a.size(); i++) {
            p[i] += a[i];
        }
        for (auto i = 0U; i < b.size(); i++) {
            p[i] -= b[i];
        }
        return p;
    }

    friend Polynomial operator-(const Polynomial &a) {
        int n = a.size();
        Polynomial p(n);
        for (int i = 0; i < n; i++) {
            p[i] = -a[i];
        }
        return p;
    }

    friend Polynomial operator*(T a, Polynomial b) {
        for (auto i = 0U; i < b.size(); i++) {
            b[i] *= a;
        }
        return b;
    }

    friend Polynomial operator*(Polynomial a, T b) {
        for (auto i = 0U; i < a.size(); i++) {
            a[i] *= b;
        }
        return a;
    }

    friend Polynomial operator/(Polynomial a, T b) {
        b = b.inv();
        for (auto i = 0U; i < a.size(); i++) {
            a[i] *= b;
        }
        return a;
    }

    Polynomial mulxk(int k) const {
        assert(k >= 0);
        Polynomial b = self;
        b.insert(b.begin(), k, 0);
        return b;
    }

    Polynomial divxk(int k) const {
        assert(k >= 0);
        if (static_cast<int>(self.size()) <= k) {
            return Polynomial{};
        }
        return Polynomial(self.begin() + k, self.end());
    }

    T whenXis(T x) const {
        T ans = T{};
        for (int i = static_cast<int>(self.size()) - 1; i >= 0; i--) {
            ans = ans * x + self[i];
        }
        return ans;
    }

    Polynomial &operator+=(Polynomial b) {
        return self = self + b;
    }
    Polynomial &operator-=(Polynomial b) {
        return self = self - b;
    }
    Polynomial &operator*=(Polynomial b) {
        return self = self * b;
    }
    Polynomial &operator*=(T b) {
        return self = self * b;
    }
    Polynomial &operator/=(T b) {
        return self = self / b;
    }

    friend Polynomial operator*(const Polynomial &a, const Polynomial &b) {
        if (a.size() == 0 or b.size() == 0) {
            return Polynomial();
        }

        int n = a.size() + b.size() - 1;
        int s = 1 << std::__lg(2 * n - 1);
        if (((P - 1) & (s - 1)) != 0 or std::min(a.size(), b.size()) < 128) {
            Polynomial p(n);
            for (auto i = 0U; i < a.size(); i++) {
                for (auto j = 0U; j < b.size(); j++) {
                    p[i + j] += a[i] * b[j];
                }
            }

            return p;
        }

        Polynomial f = a.mod(s);
        Polynomial g = b.mod(s);
        dft(f), dft(g);
        for (int i = 0; i < s; i++) {
            f[i] *= g[i];
        }
        idft(f);
        return f.mod(n);
    }

    Polynomial deriv() const {
        int n = self.size();
        if (n <= 1) {
            return Polynomial();
        }
        Polynomial p(n - 1);
        for (int i = 1; i < n; i++) {
            p[i - 1] = i * self[i];
        }
        return p;
    }

    Polynomial integr() const {
        int n = self.size();
        Polynomial p(n + 1);
        std::vector<T> _inv(n + 1);
        _inv[1] = 1;
        for (int i = 2; i <= n; i++) {
            _inv[i] = _inv[P % i] * (P - P / i);
        }
        for (int i = 0; i < n; ++i) {
            p[i + 1] = self[i] * _inv[i + 1];
        }
        return p;
    }

    // assert(self[0] != 0);
    Polynomial inv(int m = -1) const {
        const int n = self.size();
        m = m < 0 ? n : m;
        Polynomial p = Polynomial{self.at(0).inv()};
        p.reserve(4 * m);
        for (int k = 2; k / 2 < m; k <<= 1) {
            Polynomial q = Polynomial(self.begin(), self.begin() + std::min(k, n)).mod(2 * k);
            p.resize(2 * k);
            dft(q), dft(p);
            for (int i = 0; i < 2 * k; i++) {
                p[i] = p[i] * (2 - p[i] * q[i]);
            }
            idft(p);
            p.resize(k);
        }
        return p.mod(m);
    }

    Polynomial ln(int m = -1) const {
        m = m < 0 ? self.size() : m;
        return (deriv() * inv(m)).integr().mod(m);
    }

    Polynomial exp(int m = -1) const {
        m = m < 0 ? self.size() : m;
        Polynomial p{1};
        int k = 1;
        while (k < m) {
            k <<= 1;
            p = (p * (Polynomial{1} - p.ln(k) + mod(k))).mod(k);
        }
        return p.mod(m);
    }

    Polynomial power(long long k, int m = -1) const {
        m = m < 0 ? self.size() : m;
        assert(0 <= k);
        k = k % P;
        if (0 <= k and k < 6) {
            Polynomial p = self;
            Polynomial ans{1};
            for (; k; k /= 2) {
                if (k & 1) {
                    ans = (ans * p).mod(m);
                }
                p = (p * p).mod(m);
            }
            return ans.mod(m);
        }

        unsigned int i = 0;
        while (i < self.size() and self[i] == T{}) {
            i += 1;
        }
        if (i == self.size() or __int128(k) * i >= m) {
            return Polynomial(m, T{});
        }
        T v = self[i];
        Polynomial f = divxk(i) / v;
        return (f.ln(m - i * k) * k).exp(m - i * k).mulxk(i * k) * v.power(k);
    }

    Polynomial sqrt(int m = -1) const {
        m = m < 0 ? self.size() : m;
        Polynomial p{1};
        int k = 1;
        const T INV2 = T(1) / 2;
        while (k < m) {
            k <<= 1;
            p = (p + (mod(k) * p.inv(k)).mod(k)) * INV2;
        }
        return p.mod(m);
    }

    template <class Input>
    friend Input &operator>>(Input &is, Polynomial &a) {
        int n = a.size();
        for (int i = 0; i < n; i++) {
            is >> a[i];
        }
        return is;
    }

    template <class Output>
    friend Output &operator<<(Output &os, const Polynomial &a) {
        int n = a.size();
        if (n >= 1) {
            os << a[0];
        }
        for (int i = 1; i < n; i++) {
            os << ' ' << a[i];
        }
        return os;
    }

    static Polynomial prod(const std::vector<Polynomial> &p) {
        if (p.size() == 0) return {1};
        if (p.size() == 1) return {p[0]};
        if (p.size() == 2) return {p[0] * p[1]};

        const int n = p.size();
        auto dfs = [&](auto self, int l, int r) -> Polynomial {
            if (r - l == 1) {
                return p[l];
            }
            int m = (l + r) / 2;
            return self(self, l, m) * self(self, m, r);
        };
        return dfs(dfs, 0, n);
    }
    #undef self
};
template <class T>
std::vector<T> Polynomial<T>::w;

using Poly = Polynomial<Z>;

