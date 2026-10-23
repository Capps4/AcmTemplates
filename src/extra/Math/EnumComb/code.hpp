#pragma once
#include "../../../../Headers/Headers.hpp"

class EnumComb {
    using U = std::uint64_t;
    const int m, n;

public:
    struct Iterator {
        U cur;
        const int bits;
        bool done;

        U operator*() const { return cur; }

        Iterator& operator++() {
            if (cur == 0) {
                done = true;
                return *this;
            }
            U lo = cur & (U(0) - cur), hi = cur + lo;
            if (hi == 0 or (bits < 64 and hi >= (U(1) << bits))) {
                done = true;
            } else {
                cur = hi | (((hi ^ cur) >> 2) / lo);
            }
            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return done == rhs.done and (done or cur == rhs.cur);
        }
        bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }
    };

    EnumComb(int m, int n) : m(m), n(n) {
        assert(0 <= n and n <= m and m <= 64);
    }

    Iterator begin() const {
        U cur = n == 64 ? ~U(0) : (U(1) << n) - 1;
        return {cur, m, false};
    }
    Iterator end() const { return {0, m, true}; }
};
