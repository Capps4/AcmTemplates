#pragma once
#include "../../../Headers/Headers.hpp"

// SNIPPET BEGIN
// 0-based half-open query [l, r); id is the original query index.
struct Query {
    int l, r, id;

    Query(int l, int r, int id) : l(l), r(r), id(id) {}

    friend bool operator<(const Query &a, const Query &b) {
        int x = a.l / B, y = b.l / B;
        if (x != y)
            return x < y;
        return x % 2 == 0 ? a.r < b.r : a.r > b.r;
    }

    static void rangeScale(int n) {
        assert(n >= 0);
        B = int(std::sqrt(2.0 * n)) + 1;
    }

private:
    inline static int B = 500;
};
