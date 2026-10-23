#pragma once
#include "Include.hpp"

// SNIPPET BEGIN
namespace _ntt {
// Z must have a compile-time fixed prime modulus.
constexpr auto root() {
    using V = Z::ValueType;
    constexpr V p = Z::getMod();
    static_assert(p >= 2, "NTT modulus must be at least 2");
    if (p == 2) return V(1);
    std::array<V, 20> div{};
    int cnt = 0;
    V x = p - 1;
    for (V i = 2; i <= x / i; ++i) {
        if (x % i != 0) continue;
        div[cnt++] = i;
        while (x % i == 0) x /= i;
    }
    if (x > 1) div[cnt++] = x;
    for (V g = 2; g < p; ++g) {
        bool ok = true;
        for (int i = 0; i < cnt and ok; ++i)
            ok = Z(g).power((p - 1) / div[i]) != Z(1);
        if (ok) return g;
    }
    return V(0);
}

struct Poly : public std::vector<Z> {
    inline static std::vector<Z> w;

    static bool canTransform(int n) {
        return n > 0 and (n & (n - 1)) == 0 and (P - 1) % n == 0;
    }

    static constexpr auto P = Z::getMod();
    static constexpr auto G = root();
    static_assert(G != 0, "NTT primitive root not found");

    static void initW(int r) {
        assert(canTransform(r));
        if (static_cast<int>(w.size()) >= r) {
            return;
        }

        w.assign(r, 0);
        w[r >> 1] = 1;
        Z s = Z(G).power((P - 1) / r);
        for (int i = r / 2 + 1; i < r; i++) {
            w[i] = w[i - 1] * s;
        }
        for (int i = r / 2 - 1; i > 0; i--) {
            w[i] = w[i * 2];
        }
    }

    friend void dft(Poly &a) {
        const int n = int(a.size());
        if (n == 0)
            return;
        assert(canTransform(n));
        initW(n);

        for (int k = n >> 1; k; k >>= 1) {
            for (int i = 0; i < n; i += k << 1) {
                for (int j = 0; j < k; j++) {
                    Z v = a[i + j + k];
                    a[i + j + k] = (a[i + j] - v) * w[k + j];
                    a[i + j] = a[i + j] + v;
                }
            }
        }
    }

    friend void idft(Poly &a) {
        const int n = int(a.size());
        if (n == 0)
            return;
        assert(canTransform(n));
        initW(n);

        for (int k = 1; k < n; k <<= 1) {
            for (int i = 0; i < n; i += k << 1) {
                for (int j = 0; j < k; j++) {
                    Z x = a[i + j];
                    Z y = a[i + j + k] * w[j + k];
                    a[i + j + k] = x - y;
                    a[i + j] = x + y;
                }
            }
        }

        a *= P - (P - 1) / n;
        std::reverse(a.begin() + 1, a.end());
    }

public:
    using std::vector<Z>::vector;

    Poly mod(int k) const {
        assert(k >= 0);
        Poly p(this->begin(), this->begin() + std::min(k, int(this->size())));
        p.resize(k);
        return p;
    }

    friend Poly operator+(const Poly &a, const Poly &b) {
        Poly p(std::max(a.size(), b.size()));
        for (int i = 0; i < int(a.size()); i++) {
            p[i] += a[i];
        }
        for (int i = 0; i < int(b.size()); i++) {
            p[i] += b[i];
        }
        return p;
    }

    friend Poly operator-(const Poly &a, const Poly &b) {
        Poly p(std::max(a.size(), b.size()));
        for (int i = 0; i < int(a.size()); i++) {
            p[i] += a[i];
        }
        for (int i = 0; i < int(b.size()); i++) {
            p[i] -= b[i];
        }
        return p;
    }

    friend Poly operator-(const Poly &a) {
        int n = a.size();
        Poly p(n);
        for (int i = 0; i < n; i++) {
            p[i] = -a[i];
        }
        return p;
    }

    friend Poly operator*(Z a, Poly b) {
        for (int i = 0; i < int(b.size()); i++) {
            b[i] *= a;
        }
        return b;
    }

    friend Poly operator*(Poly a, Z b) {
        for (int i = 0; i < int(a.size()); i++) {
            a[i] *= b;
        }
        return a;
    }

    friend Poly operator/(Poly a, Z b) {
        b = b.inv();
        for (int i = 0; i < int(a.size()); i++) {
            a[i] *= b;
        }
        return a;
    }

    Poly mulxk(int k) const {
        assert(k >= 0);
        Poly b = *this;
        b.insert(b.begin(), k, 0);
        return b;
    }

    Poly divxk(int k) const {
        assert(k >= 0);
        if (static_cast<int>(this->size()) <= k) {
            return Poly{};
        }
        return Poly(this->begin() + k, this->end());
    }

    Z whenXis(Z x) const {
        Z res = 0;
        for (auto it = this->rbegin(); it != this->rend(); ++it)
            res = res * x + *it;
        return res;
    }

    Poly &operator+=(const Poly &b) & {
        if (this->size() < b.size())
            this->resize(b.size());
        for (int i = 0; i < int(b.size()); ++i)
            (*this)[i] += b[i];
        return *this;
    }

    Poly &operator-=(const Poly &b) & {
        if (this->size() < b.size())
            this->resize(b.size());
        for (int i = 0; i < int(b.size()); ++i)
            (*this)[i] -= b[i];
        return *this;
    }

    Poly &operator*=(const Poly &b) & {
        return *this = *this * b;
    }

    Poly &operator*=(Z b) & {
        for (auto &v : *this)
            v *= b;
        return *this;
    }

    Poly &operator/=(Z b) & {
        return *this *= b.inv();
    }

    friend Poly operator*(const Poly &a, const Poly &b) {
        if (a.size() == 0 or b.size() == 0) {
            return Poly();
        }

        int n = int(a.size() + b.size() - 1), s = 1;
        while (s < n)
            s *= 2;
        if (!canTransform(s) or std::min(a.size(), b.size()) < 128) {
            Poly p(n);
            for (int i = 0; i < int(a.size()); i++) {
                for (int j = 0; j < int(b.size()); j++) {
                    p[i + j] += a[i] * b[j];
                }
            }

            return p;
        }

        Poly f = a.mod(s);
        Poly g = b.mod(s);
        dft(f), dft(g);
        for (int i = 0; i < s; i++) {
            f[i] *= g[i];
        }
        idft(f);
        return f.mod(n);
    }

    Poly deriv() const {
        int n = this->size();
        if (n <= 1) {
            return Poly();
        }
        Poly p(n - 1);
        for (int i = 1; i < n; i++) {
            p[i - 1] = i * (*this)[i];
        }
        return p;
    }

    Poly integr() const {
        assert(int(this->size()) < P);
        int n = this->size();
        Poly p(n + 1);
        std::vector<Z> _inv(n + 1);
        assert(n < P);
        if (n)
            _inv[1] = 1;
        for (int i = 2; i <= n; i++) {
            _inv[i] = _inv[P % i] * (P - P / i);
        }
        for (int i = 0; i < n; ++i) {
            p[i + 1] = (*this)[i] * _inv[i + 1];
        }
        return p;
    }

    Poly inv(int m = -1) const {
        const int n = int(this->size());
        m = m < 0 ? n : m;
        if (m == 0)
            return {};
        assert(n > 0 and (*this)[0] != Z(0));
        Poly p{(*this)[0].inv()};
        int cap = 1;
        while (cap < m)
            cap *= 2;
        p.reserve(cap * (canTransform(2 * cap) ? 2 : 1));
        for (int k = 2; k / 2 < m; k *= 2) {
            if (canTransform(2 * k)) {
                Poly q(this->begin(), this->begin() + std::min(k, int(this->size())));
                q.resize(2 * k);
                p.resize(2 * k);
                dft(q);
                dft(p);
                for (int i = 0; i < 2 * k; ++i)
                    p[i] *= 2 - p[i] * q[i];
                idft(p);
                p.resize(k);
            } else {
                Poly q = (mod(k) * p).mod(k);
                for (auto &v : q)
                    v = -v;
                q[0] += 2;
                p = (p * q).mod(k);
            }
        }
        return p.mod(m);
    }

    Poly ln(int m = -1) const {
        m = m < 0 ? this->size() : m;
        assert(m <= P);
        if (m == 0)
            return {};
        assert(!this->empty() and (*this)[0] == Z(1));
        return (mod(m).deriv() * inv(m)).mod(m - 1).integr();
    }

    Poly exp(int m = -1) const {
        m = m < 0 ? this->size() : m;
        assert(m <= P);
        if (m == 0)
            return {};
        assert(this->empty() or (*this)[0] == Z(0));
        Poly p{1};
        int k = 1;
        while (k < m) {
            k = std::min(2 * k, m);
            p = (p * (Poly{1} - p.ln(k) + mod(k))).mod(k);
        }
        return p.mod(m);
    }

    Poly power(long long k, int m = -1) const {
        m = m < 0 ? this->size() : m;
        assert(0 <= k);
        assert(m <= P);
        if (m == 0)
            return {};
        if (0 <= k and k < 6) {
            Poly p = mod(m);
            Poly ans{1};
            for (; k; k /= 2) {
                if (k & 1) {
                    ans = (ans * p).mod(m);
                }
                p = (p * p).mod(m);
            }
            return ans.mod(m);
        }

        int i = 0;
        while (i < int(this->size()) and (*this)[i] == Z{}) {
            i += 1;
        }
        if (i == int(this->size()) or __int128(k) * i >= m) {
            return Poly(m, Z{});
        }
        Z v = (*this)[i];
        Poly f = divxk(i) / v;
        int d = int(i * k);
        return (f.ln(m - d) * Z(k)).exp(m - d).mulxk(d) * v.power(k);
    }

    Poly sqrt(int m = -1) const {
        m = m < 0 ? this->size() : m;
        if (m == 0)
            return {};
        assert(!this->empty() and (*this)[0] == Z(1));
        Poly p{1};
        int k = 1;
        const Z INV2 = Z(1) / 2;
        while (k < m) {
            k = std::min(2 * k, m);
            p = (p + (mod(k) * p.inv(k)).mod(k)) * INV2;
        }
        return p.mod(m);
    }

    template <class Input>
    friend Input &operator>>(Input &is, Poly &a) {
        int n = a.size();
        for (int i = 0; i < n; i++) {
            is >> a[i];
        }
        return is;
    }

    template <class Output>
    friend Output &operator<<(Output &os, const Poly &a) {
        int n = a.size();
        if (n >= 1) {
            os << a[0];
        }
        for (int i = 1; i < n; i++) {
            os << ' ' << a[i];
        }
        return os;
    }

    static Poly prod(const std::vector<Poly> &p) {
        if (p.size() == 0)
            return {1};
        if (p.size() == 1)
            return {p[0]};
        if (p.size() == 2)
            return {p[0] * p[1]};

        const int n = p.size();
        auto dfs = [&](auto &&self, int l, int r) -> Poly {
            if (r - l == 1) {
                return p[l];
            }
            int m = l + (r - l) / 2;
            return self(self, l, m) * self(self, m, r);
        };
        return dfs(dfs, 0, n);
    }
};

} // namespace _ntt

using Poly = _ntt::Poly;
