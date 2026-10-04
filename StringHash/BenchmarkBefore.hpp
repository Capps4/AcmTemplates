#pragma once
#include <array>
#include <cassert>
#include <chrono>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

// SNIPPET BEGIN
using u64 = unsigned long long;

template <int D, const int *B, const int *P>
class StringHashImpl {
    static_assert(D > 0 && B != nullptr && P != nullptr);
    using Hash = std::array<int, D>;
    std::vector<Hash> h{};
    inline static std::vector<Hash> powers{[] {
        Hash one{};
        one.fill(1);
        return one;
    }()};

    template <class Sequence>
    static auto symbol(const Sequence &s, int i) {
        if constexpr (std::is_integral_v<std::decay_t<decltype(s[i])>>)
            return s[i];
        else {
            static_assert(std::is_same_v<typename Sequence::value_type, bool>,
                          "requires integral symbols");
            return bool(s[i]); // libc++ may return a proxy even from const vector<bool>.
        }
    }

    static void ensurePowers(int len) {
        if (int(powers.size()) > len) return;
        int old = int(powers.size());
        powers.resize(len + 1);
        for (int i = old; i <= len; ++i)
            for (int k = 0; k < D; ++k)
                powers[i][k] = int(1LL * powers[i - 1][k] * B[k] % P[k]);
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
    template <class Sequence, class = decltype(std::declval<const Sequence &>().size())>
    explicit StringHashImpl(const Sequence &s) {
        for (int k = 0; k < D; ++k)
            assert(P[k] > 1 && 0 < B[k] && B[k] < P[k]);
        h.resize(s.size() + 1);
        for (int i = 0; i < int(s.size()); ++i)
            for (int k = 0; k < D; ++k)
                h[i + 1][k] = int((1LL * h[i][k] * B[k] + encode(symbol(s, i), P[k])) % P[k]);
    }

    explicit StringHashImpl(const char *text) : StringHashImpl(std::string_view(text)) {}

    int size() const { return int(h.size() - 1); }

    Hash getArray(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        ensurePowers(r - l);
        Hash res{};
        for (int k = 0; k < D; ++k) {
            res[k] = int(h[r][k] - 1LL * h[l][k] * powers[r - l][k] % P[k]);
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

inline constexpr int HashDimension = 2;
inline constexpr int HashMod[HashDimension] = {1000000021, 1000000097};

namespace strHashDetail {
constexpr u64 mix(u64 x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

inline const u64 seed = u64(std::chrono::steady_clock::now().time_since_epoch().count());
} // namespace strHashDetail

inline const int HashBase[HashDimension] = {
    2 + int(strHashDetail::mix(strHashDetail::seed) % (HashMod[0] - 3)),
    2 + int(strHashDetail::mix(strHashDetail::seed + 1) % (HashMod[1] - 3))};
using StringHash = StringHashImpl<HashDimension, HashBase, HashMod>;
