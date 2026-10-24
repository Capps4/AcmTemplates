#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
namespace _ge {
template <class T, class = void>
struct HasInverse : std::false_type {};

template <class T>
struct HasInverse<T, std::void_t<decltype(std::declval<const T &>().inv())>>
    : std::is_convertible<decltype(std::declval<const T &>().inv()), T> {};

template <class T>
std::vector<int> elim(std::vector<std::vector<T>> &a, int m) {
    static_assert(!std::is_integral_v<T>,
                  "Gaussian elimination needs a field or floating-point type");
    assert(m >= 0);
    int n = int(a.size()), w = n == 0 ? m : int(a[0].size());
    assert(w >= m);
    for (const auto &row : a)
        assert(int(row.size()) == w);
    std::vector<int> piv;
    piv.reserve(std::min(n, m));
    int r = 0;
    for (int c = 0; c < m and r < n; ++c) {
        int p = -1;
        for (int row = r; row < n; ++row)
            if (a[row][c] != T{}) {
                if constexpr (std::is_floating_point_v<T>) {
                    if (p == -1 or std::abs(a[row][c]) > std::abs(a[p][c]))
                        p = row;
                } else
                    p = row;
            }
        if (p == -1)
            continue;
        std::swap(a[p], a[r]);
        auto &u = a[r];
        T d = u[c];
        u[c] = T(1);
        if constexpr (_ge::HasInverse<T>::value) {
            T iv = d.inv();
            for (int j = c + 1; j < w; ++j)
                u[j] *= iv;
        } else {
            for (int j = c + 1; j < w; ++j)
                u[j] /= d;
        }
        for (int row = r + 1; row < n; ++row) {
            auto &cur = a[row];
            T v = cur[c];
            if (v == T{})
                continue;
            cur[c] = T{};
            for (int j = c + 1; j < w; ++j)
                cur[j] -= u[j] * v;
        }
        piv.push_back(c);
        ++r;
    }
    return piv;
}
} // namespace _ge

template <class T>
struct GaussResult {
    int rank = 0;
    std::vector<int> piv{};
    std::vector<std::string> status{};
    std::vector<std::vector<T>> sol{};
};

template <class T>
GaussResult<T> gauss(std::vector<std::vector<T>> &a, int m) {
    GaussResult<T> res;
    res.piv = _ge::elim(a, m);
    res.rank = int(res.piv.size());
    int n = int(a.size()), w = n == 0 ? m : int(a[0].size());
    for (int k = m; k < w; ++k) {
        bool ok = true;
        for (int i = res.rank; i < n; ++i)
            if (a[i][k] != T{})
                ok = false;
        res.status.push_back(!ok ? "NoSolution" : res.rank == m ? "OK" : "InfSolution");
        res.sol.emplace_back(ok ? m : 0);
        if (!ok)
            continue;
        auto &x = res.sol.back();
        for (int i = res.rank - 1; i >= 0; --i) {
            T v = a[i][k];
            for (int j = res.piv[i] + 1; j < m; ++j)
                v -= a[i][j] * x[j];
            x[res.piv[i]] = v;
        }
    }
    return res;
}

template <class T>
std::string gauss(std::vector<std::vector<T>> &a) {
    if (a.empty())
        return "OK";
    int n = int(a.size()), m = int(a[0].size());
    assert(m > n);
    auto piv = _ge::elim(a, n);
    int r = int(piv.size());
    if (r < n) {
        for (int row = r; row < n; ++row)
            for (int c = n; c < m; ++c)
                if (a[row][c] != T{})
                    return "NoSolution";
        return "InfSolution";
    }
    for (int row = n - 1; row > 0; --row)
        for (int pre = 0; pre < row; ++pre) {
            T v = a[pre][row];
            if (v == T{})
                continue;
            for (int c = n; c < m; ++c)
                a[pre][c] -= v * a[row][c];
            a[pre][row] = T{};
        }
    return "OK";
}

template <class T>
struct MatrixUtil {
    std::string status{};
    std::vector<std::vector<T>> inv{};

    explicit MatrixUtil(const std::vector<std::vector<T>> &a) {
        int n = int(a.size());
        for (const auto &row : a)
            assert(int(row.size()) == n);
        std::vector<std::vector<T>> b(n, std::vector<T>(2 * n));
        for (int row = 0; row < n; ++row) {
            std::copy(a[row].begin(), a[row].end(), b[row].begin());
            b[row][n + row] = T(1);
        }
        status = ::gauss(b);
        if (status == "OK") {
            inv.reserve(n);
            for (const auto &row : b)
                inv.emplace_back(row.begin() + n, row.end());
        }
    }
};
