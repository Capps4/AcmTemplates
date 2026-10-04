#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <type_traits>
#include <vector>

// SNIPPET BEGIN
template <class T, template <class> class Complex>
class FftPolynomial : public std::vector<T> {
    static_assert(std::is_floating_point_v<T>);
    using Comp = Complex<T>;
    inline static std::vector<Comp> roots;
    inline static std::vector<int> reverseIndex;

    static void init(int n) {
        if (int(reverseIndex.size()) == n) return;
        int lg = 0;
        while ((1 << lg) < n)
            ++lg;
        reverseIndex.assign(n, 0);
        for (int i = 1; i < n; ++i)
            reverseIndex[i] = (reverseIndex[i >> 1] >> 1) | ((i & 1) << (lg - 1));
        roots.resize(n / 2);
        const T twoPi = 2 * std::acos(T(-1));
        for (int i = 0; i < n / 2; ++i) {
            T ang = twoPi * i / n;
            roots[i] = Comp(std::cos(ang), std::sin(ang));
        }
    }

    template <bool Inverse>
    static void fft(std::vector<Comp> &a) {
        int n = int(a.size());
        init(n);
        for (int i = 0; i < n; ++i)
            if (i < reverseIndex[i]) std::swap(a[i], a[reverseIndex[i]]);
        for (int mid = 1; mid < n; mid *= 2) {
            int d = n / (2 * mid);
            for (int s = 0; s < n; s += 2 * mid) {
                for (int k = 0; k < mid; ++k) {
                    Comp v = roots[d * k];
                    if constexpr (Inverse) v = Comp(v.real(), -v.imag());
                    Comp x = a[s + k], y = v * a[s + mid + k];
                    a[s + k] = x + y;
                    a[s + mid + k] = x - y;
                }
            }
        }
    }

public:
    using std::vector<T>::vector;

    friend FftPolynomial operator*(const FftPolynomial &a, const FftPolynomial &b) {
        if (a.empty() || b.empty()) return {};
        int cnt = int(a.size()) + int(b.size()) - 1;
        FftPolynomial res(cnt);
        if (std::min(a.size(), b.size()) < 128) {
            for (int i = 0; i < int(a.size()); ++i)
                for (int j = 0; j < int(b.size()); ++j)
                    res[i + j] += a[i] * b[j];
            return res;
        }
        int size = 1;
        while (size < cnt)
            size *= 2;
        std::vector<Comp> p(size), q(size);
        for (int i = 0; i < int(a.size()); ++i)
            p[i] = Comp(a[i], 0);
        for (int i = 0; i < int(b.size()); ++i)
            q[i] = Comp(b[i], 0);
        fft<false>(p);
        fft<false>(q);
        for (int i = 0; i < size; ++i)
            p[i] *= q[i];
        fft<true>(p);
        for (int i = 0; i < cnt; ++i) {
            res[i] = p[i].real() / size;
            // res[i] = std::round(res[i]); // 整数系数卷积时可启用。
        }
        return res;
    }
};

using FftReal = double;
using FftPoly = FftPolynomial<FftReal, std::complex>;
