#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>
#include "../ModuloInteger/Final.hpp"

// SNIPPET BEGIN
// Fixed prime modulus; PrimitiveRoot must be a primitive root of that field.
template <class T, int PrimitiveRoot = 3>
struct NttPolynomial : public std::vector<T> {
    inline static std::vector<T> w;

    static bool canTransform(int n) { return n > 0 && (n & (n - 1)) == 0 && (P - 1) % n == 0; }

    static constexpr auto P = T::getMod();
    static_assert(P > 2 && P % 2 == 1, "requires a fixed odd prime modulus");
    static_assert(PrimitiveRoot > 0 && PrimitiveRoot < P, "supply a valid primitive root");

    static void initW(int r) {
        assert(canTransform(r));
        if (static_cast<int>(w.size()) >= r) {
            return;
        }

        w.assign(r, 0);
        w[r >> 1] = 1;
        T s = T(PrimitiveRoot).power((P - 1) / r);
        for (int i = r / 2 + 1; i < r; i++) {
            w[i] = w[i - 1] * s;
        }
        for (int i = r / 2 - 1; i > 0; i--) {
            w[i] = w[i * 2];
        }
    }

    friend void dft(NttPolynomial &a) {
        const int n = int(a.size());
        if (n == 0) return;
        assert(canTransform(n));
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

    friend void idft(NttPolynomial &a) {
        const int n = int(a.size());
        if (n == 0) return;
        assert(canTransform(n));
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

    NttPolynomial mod(int k) const {
        assert(k >= 0);
        NttPolynomial p(this->begin(), this->begin() + std::min(k, int(this->size())));
        p.resize(k);
        return p;
    }

    friend NttPolynomial operator+(const NttPolynomial &a, const NttPolynomial &b) {
        NttPolynomial p(std::max(a.size(), b.size()));
        for (int i = 0; i < int(a.size()); i++) {
            p[i] += a[i];
        }
        for (int i = 0; i < int(b.size()); i++) {
            p[i] += b[i];
        }
        return p;
    }

    friend NttPolynomial operator-(const NttPolynomial &a, const NttPolynomial &b) {
        NttPolynomial p(std::max(a.size(), b.size()));
        for (int i = 0; i < int(a.size()); i++) {
            p[i] += a[i];
        }
        for (int i = 0; i < int(b.size()); i++) {
            p[i] -= b[i];
        }
        return p;
    }

    friend NttPolynomial operator-(const NttPolynomial &a) {
        int n = a.size();
        NttPolynomial p(n);
        for (int i = 0; i < n; i++) {
            p[i] = -a[i];
        }
        return p;
    }

    friend NttPolynomial operator*(T a, NttPolynomial b) {
        for (int i = 0; i < int(b.size()); i++) {
            b[i] *= a;
        }
        return b;
    }

    friend NttPolynomial operator*(NttPolynomial a, T b) {
        for (int i = 0; i < int(a.size()); i++) {
            a[i] *= b;
        }
        return a;
    }

    friend NttPolynomial operator/(NttPolynomial a, T b) {
        b = b.inv();
        for (int i = 0; i < int(a.size()); i++) {
            a[i] *= b;
        }
        return a;
    }

    NttPolynomial mulxk(int k) const {
        assert(k >= 0);
        NttPolynomial b = *this;
        b.insert(b.begin(), k, 0);
        return b;
    }

    NttPolynomial divxk(int k) const {
        assert(k >= 0);
        if (static_cast<int>(this->size()) <= k) {
            return NttPolynomial{};
        }
        return NttPolynomial(this->begin() + k, this->end());
    }

    T whenXis(T x) const {
        T res = 0;
        for (auto it = this->rbegin(); it != this->rend(); ++it)
            res = res * x + *it;
        return res;
    }

    NttPolynomial &operator+=(const NttPolynomial &b) & {
        if (this->size() < b.size()) this->resize(b.size());
        for (int i = 0; i < int(b.size()); ++i)
            (*this)[i] += b[i];
        return *this;
    }

    NttPolynomial &operator-=(const NttPolynomial &b) & {
        if (this->size() < b.size()) this->resize(b.size());
        for (int i = 0; i < int(b.size()); ++i)
            (*this)[i] -= b[i];
        return *this;
    }

    NttPolynomial &operator*=(const NttPolynomial &b) & { return *this = *this * b; }

    NttPolynomial &operator*=(T b) & {
        for (auto &v : *this)
            v *= b;
        return *this;
    }

    NttPolynomial &operator/=(T b) & { return *this *= b.inv(); }

    friend NttPolynomial operator*(const NttPolynomial &a, const NttPolynomial &b) {
        if (a.size() == 0 or b.size() == 0) {
            return NttPolynomial();
        }

        int n = int(a.size() + b.size() - 1), s = 1;
        while (s < n)
            s *= 2;
        if (!canTransform(s) || std::min(a.size(), b.size()) < 128) {
            NttPolynomial p(n);
            for (int i = 0; i < int(a.size()); i++) {
                for (int j = 0; j < int(b.size()); j++) {
                    p[i + j] += a[i] * b[j];
                }
            }

            return p;
        }

        NttPolynomial f = a.mod(s);
        NttPolynomial g = b.mod(s);
        dft(f), dft(g);
        for (int i = 0; i < s; i++) {
            f[i] *= g[i];
        }
        idft(f);
        return f.mod(n);
    }

    NttPolynomial deriv() const {
        int n = this->size();
        if (n <= 1) {
            return NttPolynomial();
        }
        NttPolynomial p(n - 1);
        for (int i = 1; i < n; i++) {
            p[i - 1] = i * (*this)[i];
        }
        return p;
    }

    NttPolynomial integr() const {
        assert(int(this->size()) < P);
        int n = this->size();
        NttPolynomial p(n + 1);
        std::vector<T> _inv(n + 1);
        assert(n < P);
        if (n) _inv[1] = 1;
        for (int i = 2; i <= n; i++) {
            _inv[i] = _inv[P % i] * (P - P / i);
        }
        for (int i = 0; i < n; ++i) {
            p[i + 1] = (*this)[i] * _inv[i + 1];
        }
        return p;
    }

    NttPolynomial inv(int m = -1) const {
        const int n = int(this->size());
        m = m < 0 ? n : m;
        if (m == 0) return {};
        assert(n > 0 && (*this)[0] != T(0));
        NttPolynomial p{(*this)[0].inv()};
        int cap = 1;
        while (cap < m)
            cap *= 2;
        p.reserve(cap * (canTransform(2 * cap) ? 2 : 1));
        for (int k = 2; k / 2 < m; k *= 2) {
            if (canTransform(2 * k)) {
                NttPolynomial q(this->begin(),
                                this->begin() + std::min(k, int(this->size())));
                q.resize(2 * k);
                p.resize(2 * k);
                dft(q);
                dft(p);
                for (int i = 0; i < 2 * k; ++i)
                    p[i] *= 2 - p[i] * q[i];
                idft(p);
                p.resize(k);
            } else {
                NttPolynomial q = (mod(k) * p).mod(k);
                for (auto &v : q)
                    v = -v;
                q[0] += 2;
                p = (p * q).mod(k);
            }
        }
        return p.mod(m);
    }

    NttPolynomial ln(int m = -1) const {
        m = m < 0 ? this->size() : m;
        assert(m <= P);
        if (m == 0) return {};
        assert(!this->empty() && (*this)[0] == T(1));
        return (mod(m).deriv() * inv(m)).mod(m - 1).integr();
    }

    NttPolynomial exp(int m = -1) const {
        m = m < 0 ? this->size() : m;
        assert(m <= P);
        if (m == 0) return {};
        assert(this->empty() || (*this)[0] == T(0));
        NttPolynomial p{1};
        int k = 1;
        while (k < m) {
            k = std::min(2 * k, m);
            p = (p * (NttPolynomial{1} - p.ln(k) + mod(k))).mod(k);
        }
        return p.mod(m);
    }

    NttPolynomial power(long long k, int m = -1) const {
        m = m < 0 ? this->size() : m;
        assert(0 <= k);
        assert(m <= P);
        if (m == 0) return {};
        if (0 <= k and k < 6) {
            NttPolynomial p = mod(m);
            NttPolynomial ans{1};
            for (; k; k /= 2) {
                if (k & 1) {
                    ans = (ans * p).mod(m);
                }
                p = (p * p).mod(m);
            }
            return ans.mod(m);
        }

        int i = 0;
        while (i < int(this->size()) and (*this)[i] == T{}) {
            i += 1;
        }
        if (i == int(this->size()) or __int128(k) * i >= m) {
            return NttPolynomial(m, T{});
        }
        T v = (*this)[i];
        NttPolynomial f = divxk(i) / v;
        int d = int(i * k);
        return (f.ln(m - d) * T(k)).exp(m - d).mulxk(d) * v.power(k);
    }

    NttPolynomial sqrt(int m = -1) const {
        m = m < 0 ? this->size() : m;
        if (m == 0) return {};
        assert(!this->empty() && (*this)[0] == T(1));
        NttPolynomial p{1};
        int k = 1;
        const T INV2 = T(1) / 2;
        while (k < m) {
            k = std::min(2 * k, m);
            p = (p + (mod(k) * p.inv(k)).mod(k)) * INV2;
        }
        return p.mod(m);
    }

    template <class Input>
    friend Input &operator>>(Input &is, NttPolynomial &a) {
        int n = a.size();
        for (int i = 0; i < n; i++) {
            is >> a[i];
        }
        return is;
    }

    template <class Output>
    friend Output &operator<<(Output &os, const NttPolynomial &a) {
        int n = a.size();
        if (n >= 1) {
            os << a[0];
        }
        for (int i = 1; i < n; i++) {
            os << ' ' << a[i];
        }
        return os;
    }

    static NttPolynomial prod(const std::vector<NttPolynomial> &p) {
        if (p.size() == 0) return {1};
        if (p.size() == 1) return {p[0]};
        if (p.size() == 2) return {p[0] * p[1]};

        const int n = p.size();
        auto dfs = [&](auto &&self, int l, int r) -> NttPolynomial {
            if (r - l == 1) {
                return p[l];
            }
            int m = l + (r - l) / 2;
            return self(self, l, m) * self(self, m, r);
        };
        return dfs(dfs, 0, n);
    }
};

using NttPoly = NttPolynomial<Z>;
