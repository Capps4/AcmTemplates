#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
// 0-based half-open query [l, r); id is the original query index.
struct Query {
    int l, r, id;

    Query(int l, int r, int id) : l(l), r(r), id(id) {}

};

class MoSort {
    int b;

public:
    MoSort(int n, int q) : b(1) {
        assert(n >= 0 and q >= 0);
        b = std::max(1, int(n / std::sqrt(double(std::max(1, q)))));
    }

    bool operator()(const Query &x, const Query &y) const {
        int l = x.l / b, r = y.l / b;
        if (l != r)
            return l < r;
        return l % 2 == 0 ? x.r < y.r : x.r > y.r;
    }
};
