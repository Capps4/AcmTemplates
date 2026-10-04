#pragma once
#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>
namespace heapBuckets {

template<class F>
struct Op {
    F f;
    template<class List>
    friend decltype(auto) operator|(List&& a, Op op) {
        return op.f(std::forward<List>(a));
    }
};
template<class F> Op(F) -> Op<F>;

template<class T>
constexpr bool canMove = !std::is_lvalue_reference_v<T> &&
                        !std::is_const_v<std::remove_reference_t<T>>;

template<int B, bool Descending = false, class List>
void radixSort(List& a, typename List::value_type minV,
               typename List::value_type maxV) {
    using T = typename List::value_type;
    using U = std::make_unsigned_t<T>;
    auto key = [=](T x) {
        return U(Descending ? U(maxV) - U(x) : U(x) - U(minV));
    };
    constexpr int mask = (1 << B) - 1;
    const std::size_t n = a.size();
    std::vector<T> b(n);
    std::vector<std::size_t> cnt(1 << B);
    bool inBuffer = false;
    auto range = +U(U(maxV) - U(minV));
    for (int shift = 0; range; shift += B, range >>= B) {
        auto pass = [&](auto first, auto output) {
            std::fill(cnt.begin(), cnt.end(), 0);
            for (std::size_t j = 0; j < n; ++j)
                ++cnt[(key(first[j]) >> shift) & mask];
            if (cnt[(key(first[0]) >> shift) & mask] == n)
                return false;
            for (std::size_t j = 1; j < cnt.size(); ++j)
                cnt[j] += cnt[j - 1];
            for (std::size_t j = n; j-- > 0;)
                output[--cnt[(key(first[j]) >> shift) & mask]] = first[j];
            return true;
        };
        bool changed = inBuffer ? pass(b.begin(), a.begin())
                                : pass(a.begin(), b.begin());
        inBuffer ^= changed;
    }
    if (inBuffer)
        std::move(b.begin(), b.end(), a.begin());
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
        constexpr bool ascending = std::is_same_v<Compare, std::less<T>> || std::is_same_v<Compare, std::less<>>;
        constexpr bool descending = std::is_same_v<Compare, std::greater<T>> || std::is_same_v<Compare, std::greater<>>;
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool> &&
                      (ascending || descending)) {
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
        if constexpr (ascending || descending)
            std::sort(result.begin(), result.end(), cmp);
        else
            std::sort(result.begin(), result.end(), std::ref(cmp));
        return result;
    }};
}
}
