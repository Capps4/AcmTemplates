#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// SNIPPET BEGIN
namespace gaussDetail {
template <class T, class = void>
struct HasInverse : std::false_type {};

template <class T>
struct HasInverse<T, std::void_t<decltype(std::declval<const T &>().inv())>>
    : std::is_convertible<decltype(std::declval<const T &>().inv()), T> {};
} // namespace gaussDetail

template <class T>
std::string gauss(std::vector<std::vector<T>> &a) {
    static_assert(!std::is_integral_v<T>,
                  "Gaussian elimination needs a field or floating-point type");
    if (a.empty()) return "OK";
    int n = int(a.size()), m = int(a[0].size());
    assert(m > n);
    for (const auto &row : a)
        assert(int(row.size()) == m);
    int r = 0;
    for (int c = 0; c < n && r < n; ++c) {
        int p = -1;
        for (int row = r; row < n; ++row)
            if (a[row][c] != T{}) {
                if constexpr (std::is_floating_point_v<T>) {
                    if (p == -1 || std::abs(a[row][c]) > std::abs(a[p][c])) p = row;
                } else
                    p = row;
            }
        if (p == -1) continue;
        std::swap(a[p], a[r]);
        auto &u = a[r];
        T d = u[c];
        u[c] = T(1);
        if constexpr (gaussDetail::HasInverse<T>::value) {
            T iv = d.inv();
            for (int j = c + 1; j < m; ++j)
                u[j] *= iv;
        } else {
            for (int j = c + 1; j < m; ++j)
                u[j] /= d;
        }
        for (int row = r + 1; row < n; ++row) {
            auto &cur = a[row];
            T v = cur[c];
            if (v == T{}) continue;
            cur[c] = T{};
            for (int j = c + 1; j < m; ++j)
                cur[j] -= u[j] * v;
        }
        ++r;
    }
    if (r < n) {
        for (int row = r; row < n; ++row)
            for (int c = n; c < m; ++c)
                if (a[row][c] != T{}) return "NoSolution";
        return "InfSolution";
    }
    for (int row = n - 1; row > 0; --row)
        for (int pre = 0; pre < row; ++pre) {
            T v = a[pre][row];
            if (v == T{}) continue;
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
