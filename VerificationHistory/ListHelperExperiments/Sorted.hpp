#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace seq {

template<class F>
struct Op {
    F f;
    template<class List>
    friend decltype(auto) operator|(List&& a, Op op) {
        return op.f(std::forward<List>(a));
    }
};
template<class F> Op(F) -> Op<F>;

template<class T> constexpr bool isVector = false;
template<class T, class A> constexpr bool isVector<std::vector<T, A>> = true;

template<int B, bool Descending = false, class T, class A>
void radixSort(std::vector<T, A>& a, T minV, T maxV) {
    using U = std::make_unsigned_t<T>;
    auto key = [=](T x) {
        return U(Descending ? U(maxV) - U(x) : U(x) - U(minV));
    };
    constexpr int mask = (1 << B) - 1;
    const std::size_t n = a.size();
    auto range = +U(U(maxV) - U(minV));
    std::vector<T, A> b(n, a.get_allocator());
    std::array<std::size_t, 1 << B> cnt{};
    for (int shift = 0; range; shift += B, range >>= B) {
        cnt.fill(0);
        for (auto x : a)
            ++cnt[(key(x) >> shift) & mask];
        if (cnt[(key(a.front()) >> shift) & mask] == n)
            continue;
        for (std::size_t i = 1; i < cnt.size(); ++i)
            cnt[i] += cnt[i - 1];
        for (std::size_t j = n; j-- > 0;)
            b[--cnt[(key(a[j]) >> shift) & mask]] = a[j];
        a.swap(b);
    }
}

template<class Compare = std::less<>>
auto sorted(Compare cmp = {}) {
    return Op{[cmp = std::move(cmp)](auto&& a) mutable {
        auto result = std::forward<decltype(a)>(a);
        if (result.size() < 2)
            return result;
        if (cmp(result.back(), result.front())) {
            if (std::is_sorted(result.rbegin(), result.rend(), std::ref(cmp))) {
                std::reverse(result.begin(), result.end());
                return result;
            }
        } else if (std::is_sorted(result.begin(), result.end(), std::ref(cmp))) {
            return result;
        }
        using T = typename decltype(result)::value_type;
        constexpr bool ascending = std::is_same_v<Compare, std::less<T>> ||
                                   std::is_same_v<Compare, std::less<>>;
        constexpr bool descending = std::is_same_v<Compare, std::greater<T>> ||
                                    std::is_same_v<Compare, std::greater<>>;
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool> &&
                      isVector<decltype(result)> && (ascending || descending)) {
            if (result.size() >= 256) {
                auto [lo, hi] = std::minmax_element(result.begin(), result.end());
                using U = std::make_unsigned_t<T>;
                int bits = 0;
                for (auto range = +U(U(*hi) - U(*lo)); range; range >>= 1)
                    ++bits;
                int passes = (bits + 7) / 8;
                if (result.size() >= 256u * passes) {
                    if (result.size() >= 4096 && (bits + 10) / 11 < passes)
                        radixSort<11, descending>(result, *lo, *hi);
                    else
                        radixSort<8, descending>(result, *lo, *hi);
                    return result;
                }
            }
        }
        if constexpr (std::is_integral_v<T> && ascending)
            std::sort(result.begin(), result.end());
        else if constexpr (std::is_integral_v<T> && descending)
            std::sort(result.begin(), result.end(), std::greater<>{});
        else
            std::sort(result.begin(), result.end(), std::ref(cmp));
        return result;
    }};
}

} // namespace seq

using seq::sorted;
