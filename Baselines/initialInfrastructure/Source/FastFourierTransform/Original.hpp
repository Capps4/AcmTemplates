#pragma once
#include <bits/stdc++.h>
using i64 = long long;
template <class T, template <class G> class Complex>
class Polynomial : public std::vector<T> {
    using Comp = Complex<T>;

    static std::vector<Comp> w[2];
    static std::vector<int> r;

    static void init(int _log) {
        if (r.size() == (1U << _log)) {
            return;
        }

        int n = 1 << _log;
        r.assign(n, 0);
        for (int i = 1; i < n; i++) {
            r[i] = (r[i >> 1] >> 1) | ((i & 1) << (_log - 1));
        }

        w[0].assign(n, Comp());
        w[1].assign(n, Comp());

        const T PI = acosl(-1);
        for (int i = 0; i < n; i++) {
            auto th = PI * i / n;
            auto cth = std::cos(1.L * th);
            auto sth = std::sin(1.L * th);
            w[0][i] = Comp(cth, sth);
            w[1][i] = Comp(cth, -sth);
        }
    }

    static void fft(std::vector<Comp> &a, int op) {
        int n = a.size();
        init(std::__lg(n));
        for (int i = 0; i < n; i++) {
            if (i < r[i]) {
                std::swap(a[i], a[r[i]]);
            }
        }
        for (int mid = 1; mid < n; mid <<= 1) {
            const int d = n / mid;
            for (int R = mid << 1, j = 0; j < n; j += R) {
                for (int k = 0; k < mid; k++) {
                    Comp x = a[j + k];
                    Comp y = w[op][d * k] * a[j + mid + k];
                    a[j + k] = x + y;
                    a[j + mid + k] = x - y;
                }
            }
        }
    }

public:
    using std::vector<T>::vector;

    constexpr friend Polynomial operator*(const Polynomial &a, const Polynomial &b) {
        if (a.size() == 0 or b.size() == 0) {
            return Polynomial();
        }
        int n = a.size() + b.size() - 1;
        int _log = std::__lg(2 * n - 1);
        int s = 1 << _log;
        if (std::min(a.size(), b.size()) < 128) {
            Polynomial res(n);
            for (auto i = 0U; i < a.size(); i++) {
                for (auto j = 0U; j < b.size(); j++) {
                    res[i + j] += a[i] * b[j];
                }
            }
            return res;
        }

        std::vector<Comp> p(s), q(s);
        for (auto i = 0U; i < a.size(); i++) {
            p[i] = Comp(a[i], 0);
        }
        for (auto i = 0U; i < b.size(); i++) {
            q[i] = Comp(b[i], 0);
        }

        fft(p, 0);
        fft(q, 0);
        for (int i = 0; i < s; i++) {
            p[i] = p[i] * q[i];
        }
        fft(p, 1);

        Polynomial res(n);
        for (int i = 0; i < n; i++) {
            res[i] = p[i].real() / s; // 默认浮点数
        }
        return res;
    }
};

template <class T, template <class G> class Complex>
std::vector<Complex<T>> Polynomial<T, Complex>::w[2] = {};

template <class T, template <class G> class Complex>
std::vector<int> Polynomial<T, Complex>::r;

using Float = double;
using Poly = Polynomial<Float, std::complex>;

