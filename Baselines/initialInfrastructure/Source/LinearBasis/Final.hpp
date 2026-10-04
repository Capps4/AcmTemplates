#pragma once
#include <array>
#include <cassert>
#include <limits>
#include <type_traits>

// SNIPPET BEGIN
template <class T>
class LinearBasis {
    static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>);
    static constexpr int bits = std::numeric_limits<T>::digits;
    bool dirty = false;

    void build() {
        if (!dirty) return;
        for (int i = 1; i < bits; ++i)
            for (int j = i - 1; j >= 0; --j)
                if ((b[i] >> j) & 1) b[i] ^= b[j];
        dirty = false;
    }

public:
    std::array<T, bits> b{};
    int rank = 0;
    bool canZero = false; // A nonempty subset can produce zero.
    LinearBasis() = default;

    void clear() {
        b.fill(0);
        rank = 0;
        canZero = dirty = false;
    }

    void insert(T v) {
        if constexpr (std::is_signed_v<T>) assert(v >= 0);
        for (int i = bits - 1; i >= 0; --i)
            if ((v >> i) & 1) {
                if (!b[i]) {
                    b[i] = v;
                    ++rank;
                    dirty = true;
                    return;
                }
                v ^= b[i];
            }
        canZero = true;
    }

    bool check(T v) const {
        if constexpr (std::is_signed_v<T>)
            if (v < 0) return false;
        for (int i = bits - 1; i >= 0; --i)
            if ((v >> i) & 1) {
                if (!b[i]) return false;
                v ^= b[i];
            }
        return true;
    }

    T getMax(T on = 0) const {
        if constexpr (std::is_signed_v<T>) assert(on >= 0);
        for (int i = bits - 1; i >= 0; --i)
            if (b[i] && (on ^ b[i]) > on) on ^= b[i];
        return on;
    }

    T getMin(T on = 0) const {
        if constexpr (std::is_signed_v<T>) assert(on >= 0);
        for (int i = bits - 1; i >= 0; --i)
            if (b[i] && ((on >> i) & 1)) on ^= b[i];
        return on;
    }

    // Zero-based order among distinct XOR values of nonempty subsets.
    // Requires a valid index.
    T findByOrder(T k) {
        // 若允许空集，注释掉下面这行。
        k += !canZero;
        build();
        T ans = 0;
        int j = 0;
        for (int i = 0; i < bits; ++i)
            if (b[i]) {
                if ((k >> j) & 1) ans ^= b[i];
                ++j;
            }
        return ans;
    }
};
