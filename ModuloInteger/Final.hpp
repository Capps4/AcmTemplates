#pragma once
#include <cassert>
#include <type_traits>

// SNIPPET BEGIN
// P>0: fixed modulus; P==0: shared runtime modulus for this T.
template <class T, T P>
class ModuloInteger {
    static_assert(std::is_same_v<T, int> || std::is_same_v<T, long long>);
    static_assert(P >= 0);
    using U = std::make_unsigned_t<T>;
    using Wide = std::conditional_t<sizeof(T) <= 4, long long, __int128>;
    inline static T mod = sizeof(T) <= 4 ? T(998244353) : T(4179340454199820289LL);
    T x = 0;

    static constexpr ModuloInteger raw(T v) {
        ModuloInteger res;
        res.x = v;
        return res;
    }

public:
    using ValueType = T;
    constexpr ModuloInteger() = default;

    template <class I, std::enable_if_t<std::is_integral_v<I>, int> = 0>
    constexpr ModuloInteger(I v) {
        if constexpr (std::is_signed_v<I>) {
            using Input = std::conditional_t<(sizeof(I) <= 8), long long, __int128>;
            auto rem = Input(v) % getMod();
            x = T(rem < 0 ? rem + getMod() : rem);
        } else {
            using Input =
                std::conditional_t<(sizeof(I) <= 8), unsigned long long, unsigned __int128>;
            x = T(Input(v) % U(getMod()));
        }
    }

    static constexpr T getMod() {
        if constexpr (P > 0)
            return P;
        else
            return mod;
    }

    static void setMod(T v) {
        static_assert(P == 0, "setMod requires a runtime-modulus type");
        assert(v > 0);
        mod = v;
    }

    constexpr T val() const { return x; }

    explicit constexpr operator T() const { return x; }

    constexpr ModuloInteger operator-() const { return raw(x == 0 ? 0 : getMod() - x); }

    constexpr ModuloInteger &operator+=(ModuloInteger rhs) & {
        U sum = U(x) + U(rhs.x), modulus = U(getMod());
        x = T(sum >= modulus ? sum - modulus : sum);
        return *this;
    }

    constexpr ModuloInteger &operator-=(ModuloInteger rhs) & {
        x -= rhs.x;
        if (x < 0) x += getMod();
        return *this;
    }

    constexpr ModuloInteger &operator*=(ModuloInteger rhs) & {
        x = T(Wide(x) * rhs.x % getMod());
        return *this;
    }

    // Negative k and division require a prime modulus and nonzero a/divisor.
    constexpr ModuloInteger power(long long k) const {
        ModuloInteger a = *this, res = 1;
        auto m = static_cast<unsigned long long>(k);
        if (k < 0) {
            a = inv();
            m = 0ULL - m;
        }
        for (; m; m >>= 1) {
            if (m & 1) res *= a;
            a *= a;
        }
        return res;
    }

    constexpr ModuloInteger inv() const {
        assert(x != 0);
        return power(getMod() - 2);
    }

    constexpr ModuloInteger &operator/=(ModuloInteger rhs) & { return *this *= rhs.inv(); }

    friend constexpr ModuloInteger operator+(ModuloInteger a, ModuloInteger b) { return a += b; }

    friend constexpr ModuloInteger operator-(ModuloInteger a, ModuloInteger b) { return a -= b; }

    friend constexpr ModuloInteger operator*(ModuloInteger a, ModuloInteger b) { return a *= b; }

    friend constexpr ModuloInteger operator/(ModuloInteger a, ModuloInteger b) { return a /= b; }

    friend constexpr bool operator==(ModuloInteger a, ModuloInteger b) { return a.x == b.x; }

    friend constexpr bool operator!=(ModuloInteger a, ModuloInteger b) { return a.x != b.x; }

    template <class Input>
    friend Input &operator>>(Input &in, ModuloInteger &a) {
        long long v;
        if (in >> v) a = ModuloInteger(v);
        return in;
    }

    template <class Output>
    friend Output &operator<<(Output &out, ModuloInteger v) {
        out << v.val();
        return out;
    }
};

inline constexpr int P = 998244353;
using Z = ModuloInteger<std::remove_cv_t<decltype(P)>, P>;
