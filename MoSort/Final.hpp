#pragma once
#include <cassert>
#include <cmath>

// SNIPPET BEGIN
// 0-based half-open query [l, r); id is the original query index.
struct Query {
    int l, r, id;

    Query(int l, int r, int id) : l(l), r(r), id(id) {}

    friend bool operator<(const Query &a, const Query &b) {
        int first = a.l / blockSize, second = b.l / blockSize;
        if (first != second) return first < second;
        return first % 2 == 0 ? a.r < b.r : a.r > b.r;
    }

    static void rangeScale(int n) {
        assert(n >= 0);
        blockSize = int(std::sqrt(2.0 * n)) + 1;
    }

private:
    inline static int blockSize = 500;
};
