#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T>
class ExCrt {
    std::vector<std::pair<T, T>> q{};
    using I = __int128;

    static I norm(I x, I m) {
        x %= m;
        return x < 0 ? x + m : x;
    }

    static I gcd(I a, I b, I& x, I& y) {
        if (b == 0) {
            x = 1;
            y = 0;
            return a;
        }
        I g = gcd(b, a % b, y, x);
        y -= a / b * x;
        return g;
    }

public:
    void insert(T a, T mod) {
        static_assert(std::is_integral_v<T> and std::is_signed_v<T> and sizeof(T) <= 8);
        assert(mod > 0);
        q.emplace_back(a, mod);
    }

    // Consume all equations. -1 means inconsistent; overflow throws.
    T get() {
        auto eq = std::move(q);
        q.clear();
        I res = 0, mod = 1;
        for (auto [a, m] : eq) {
            I x = 0, y = 0, g = gcd(mod, m, x, y);
            I d = norm(I(a) - res, m);
            if (d % g != 0) {
                return -1;
            }
            I k = norm(x * (d / g), I(m) / g);
            I nxt = mod / g * m;
            if (nxt > std::numeric_limits<T>::max()) {
                throw std::overflow_error("ExCrt modulus exceeds result type");
            }
            res = norm(res + mod * k, nxt);
            mod = nxt;
        }
        return T(res);
    }
};
