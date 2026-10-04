#pragma once
#include <array>
#include <cassert>
#include <chrono>
#include <random>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

// SNIPPET BEGIN
namespace _strhash {
using u64 = unsigned long long;

template <int D, const int *B, const int *P>
class Impl {
    static_assert(D > 0 && B != nullptr && P != nullptr);
    using A = std::array<int, D>;
    std::vector<A> h;
    inline static std::vector<A> pw{[] {
        A one{};
        one.fill(1);
        return one;
    }()};

    template <class List>
    static auto symbol(const List &s, int i) {
        if constexpr (std::is_integral_v<std::decay_t<decltype(s[i])>>)
            return s[i];
        else {
            static_assert(std::is_same_v<typename List::value_type, bool>, "requires integral symbols");
            return bool(s[i]); // libc++ may return a proxy even from const vector<bool>.
        }
    }

    template <class I>
    static int encode(I v, int mod) {
        static_assert(std::is_integral_v<I>);
        if constexpr (std::is_same_v<I, char> || std::is_same_v<I, signed char> ||
                      std::is_same_v<I, unsigned char>)
            return int(static_cast<unsigned char>(v)) + 1;
        else if constexpr (std::is_signed_v<I>) {
            auto rem = v % mod;
            if (rem < 0) rem += mod;
            return int(rem) + 1;
        } else {
            auto rem = v % mod;
            return int(rem) + 1;
        }
    }

public:
    template <class List, class = decltype(std::declval<const List &>().size())>
    explicit Impl(const List &s) : h(s.size() + 1) {
        for (int k = 0; k < D; ++k)
            assert(P[k] > 1 && 0 < B[k] && B[k] < P[k]);
        int n = size(), m = int(pw.size());
        if (m <= n) {
            pw.resize(n + 1);
            for (int i = m; i <= n; ++i)
                for (int k = 0; k < D; ++k)
                    pw[i][k] = int(1LL * pw[i - 1][k] * B[k] % P[k]);
        }
        for (int i = 0; i < n; ++i)
            for (int k = 0; k < D; ++k)
                h[i + 1][k] = int((1LL * h[i][k] * B[k] + encode(symbol(s, i), P[k])) % P[k]);
    }

    explicit Impl(const char *text) : Impl(std::string_view(text)) {}

    int size() const { return int(h.size() - 1); }

    A getArray(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        A res{};
        for (int k = 0; k < D; ++k) {
            res[k] = int(h[r][k] - 1LL * h[l][k] * pw[r - l][k] % P[k]);
            if (res[k] < 0) res[k] += P[k];
        }
        return res;
    }

    u64 getU64(int l, int r = -1) const {
        static_assert(D <= 2, "use getArray for more than two dimensions");
        if (r == -1) r = size();
        auto hash = getArray(l, r);
        u64 res = hash[0];
        if constexpr (D == 2) res |= u64(hash[1]) << 32;
        return res;
    }
};

inline int base(int mod) {
    static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    return std::uniform_int_distribution<int>(mod / 2 + 1, mod - 1)(rng);
}

inline constexpr int d = 2, p[d] = {1000000021, 1000000097};
inline const int b[d] = {base(p[0]), base(p[1])};
} // namespace _strhash

using StringHash = _strhash::Impl<_strhash::d, _strhash::b, _strhash::p>;
