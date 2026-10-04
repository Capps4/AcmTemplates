#pragma once
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

namespace sorting {

template<int B, bool Descending, class RandomIt, class Unsigned>
void radixSort(RandomIt first, RandomIt last, Unsigned minValue, Unsigned maxV) {
    static_assert(B > 0 && B <= 16, "radix digit width must be in [1, 16]");
    using T = typename std::iterator_traits<RandomIt>::value_type;
    const std::size_t n = last - first;
    if (!n || !maxV)
        return;
    constexpr unsigned buckets = 1u << B, mask = buckets - 1;
    std::vector<T> buffer(n);
    std::vector<std::size_t> positions(buckets);
    bool inBuffer = false;
    for (int shift = 0; maxV; shift += B) {
        auto digit = [&](T value) {
            Unsigned key = static_cast<Unsigned>(value) - minValue;
            unsigned index = (key >> shift) & mask;
            if constexpr (Descending)
                index ^= mask;
            return index;
        };
        auto pass = [&](auto source, auto target) {
            std::fill(positions.begin(), positions.end(), 0);
            for (std::size_t j = 0; j < n; ++j)
                ++positions[digit(source[j])];
            if (positions[digit(source[0])] == n)
                return false;
            std::size_t total = 0;
            for (std::size_t& position : positions) {
                std::size_t count = position;
                position = total;
                total += count;
            }
            for (std::size_t j = 0; j < n; ++j)
                target[positions[digit(source[j])]++] = source[j];
            return true;
        };
        bool scattered = inBuffer ? pass(buffer.begin(), first) : pass(first, buffer.begin());
        inBuffer ^= scattered;
        if constexpr (B < std::numeric_limits<Unsigned>::digits)
            maxV >>= B;
        else
            maxV = 0;
    }
    if (inBuffer)
        std::move(buffer.begin(), buffer.end(), first);
}

template<class RandomIt, class Compare>
void sortRange(RandomIt first, RandomIt last, Compare compare) {
    if (std::is_sorted(first, last, compare))
        return;
    if (std::is_sorted(first, last, [&](const auto& a, const auto& b) {
        return compare(b, a);
    })) {
        std::reverse(first, last);
        return;
    }
    using T = typename std::iterator_traits<RandomIt>::value_type;
    constexpr bool ascending = std::is_same_v<Compare, std::less<T>> ||
                               std::is_same_v<Compare, std::less<>>;
    constexpr bool descending = std::is_same_v<Compare, std::greater<T>> ||
                                std::is_same_v<Compare, std::greater<>>;
    if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool> && (ascending || descending)) {
        const std::size_t n = last - first;
        int logN = 0;
        for (std::size_t size = n; size > 1; size >>= 1)
            ++logN;
        const double sortWork = double(n) * logN;
        // Skip the bounds scan if even the cheapest radix estimate loses.
        if (sortWork > 2.0 * n + 256) {
            using Unsigned = std::make_unsigned_t<T>;
            auto bounds = std::minmax_element(first, last);
            // Unsigned subtraction also normalizes negative and extreme values.
            Unsigned minValue = static_cast<Unsigned>(*bounds.first);
            Unsigned maxV = static_cast<Unsigned>(*bounds.second) - minValue;
            int bits = 0;
            for (Unsigned value = maxV; value; value >>= 1)
                ++bits;
            double work8 = n + double((bits + 7) / 8) * (n + 256.0);
            double work11 = n + double((bits + 10) / 11) * (n + 2048.0);
            if (sortWork > std::min(work8, work11)) {
                if (work11 < work8)
                    radixSort<11, descending>(first, last, minValue, maxV);
                else
                    radixSort<8, descending>(first, last, minValue, maxV);
                return;
            }
        }
    }
    std::sort(first, last, compare);
}

template<class Container, class Compare = std::less<typename Container::value_type>>
auto sort(Container& a, Compare compare = {}) -> decltype(a.begin(), void()) {
    using Category = typename std::iterator_traits<decltype(a.begin())>::iterator_category;
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, Category>) {
        sorting::sortRange(a.begin(), a.end(), compare);
    } else if (!std::is_sorted(a.begin(), a.end(), compare)) {
        a.sort(compare);
    }
}

template<class Container, class Compare = std::less<typename Container::value_type>>
Container sorted(Container a, Compare compare = {}) {
    sorting::sort(a, compare);
    return a;
}

} // namespace sorting

using sorting::sort;
using sorting::sorted;
