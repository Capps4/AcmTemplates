#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <limits>
#include <type_traits>
#include <vector>

// SNIPPET BEGIN
template<class T, template<class> class Complex>
class Polynomial : public std::vector<T> {
    static_assert(std::is_floating_point_v<T>);
    using Comp = Complex<T>;
    inline static std::vector<Comp> roots;
    inline static std::vector<int> reverseIndex;

    static void init(int n) {
        if (reverseIndex.size() == std::size_t(n)) return;
        int log = 0;
        while ((1U << log) < unsigned(n)) ++log;
        reverseIndex.assign(n, 0);
        for (int i = 1; i < n; ++i)
            reverseIndex[i] = (reverseIndex[i >> 1] >> 1) | ((i & 1) << (log - 1));
        roots.resize(n / 2);
        const T twoPi = 2 * std::acos(T(-1));
        for (int i = 0; i < n / 2; ++i) {
            T angle = twoPi * i / n;
            roots[i] = Comp(std::cos(angle), std::sin(angle));
        }
    }
    template<bool Inverse>
    static void fft(std::vector<Comp>& a) {
        int n = int(a.size());
        init(n);
        for (int i = 0; i < n; ++i)
            if (i < reverseIndex[i]) std::swap(a[i], a[reverseIndex[i]]);
        for (int mid = 1; mid < n; mid *= 2) {
            int stride = n / (2 * mid);
            for (int start = 0; start < n; start += 2 * mid) {
                for (int k = 0; k < mid; ++k) {
                    Comp root = roots[stride * k];
                    if constexpr (Inverse) root = Comp(root.real(), -root.imag());
                    Comp x = a[start + k], y = root * a[start + mid + k];
                    a[start + k] = x + y;
                    a[start + mid + k] = x - y;
                }
            }
        }
    }
public:
    using std::vector<T>::vector;
    friend Polynomial operator*(const Polynomial& a, const Polynomial& b) {
        if (a.empty() || b.empty()) return {};
        assert(a.size() <= std::size_t(std::numeric_limits<int>::max()));
        assert(b.size() <= std::size_t(std::numeric_limits<int>::max()));
        auto count = a.size() + b.size() - 1;
        assert(count <= std::size_t(std::numeric_limits<int>::max()));
        Polynomial result(count);
        if (std::min(a.size(), b.size()) < 128) {
            for (std::size_t i = 0; i < a.size(); ++i)
                for (std::size_t j = 0; j < b.size(); ++j) result[i + j] += a[i] * b[j];
            return result;
        }
        assert(count <= (1U << 30));
        int size = 1;
        while (std::size_t(size) < count) size *= 2;
        std::vector<Comp> p(size), q(size);
        for (std::size_t i = 0; i < a.size(); ++i) p[i] = Comp(a[i], 0);
        for (std::size_t i = 0; i < b.size(); ++i) q[i] = Comp(b[i], 0);
        fft<false>(p);
        fft<false>(q);
        for (int i = 0; i < size; ++i) p[i] *= q[i];
        fft<true>(p);
        for (std::size_t i = 0; i < count; ++i) result[i] = p[i].real() / size;
        return result;
    }
};
using Float = double;
using Poly = Polynomial<Float, std::complex>;
