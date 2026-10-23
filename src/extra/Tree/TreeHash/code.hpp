#pragma once
#include "../../../../Headers/Headers.hpp"

class TreeHash {
    using U = std::uint64_t;
    U key;

public:
    explicit TreeHash(U seed = std::chrono::steady_clock::now().time_since_epoch().count())
        : key(seed) {}

    U operator()(U x) const {
        // Odd multipliers and xor-shifts keep all 64 input bits reversible.
        x += key + 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }
};

inline TreeHash treeHash{};
inline std::uint64_t f(std::uint64_t x) { return treeHash(x); }
