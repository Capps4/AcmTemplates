#pragma once
#include <bits/stdc++.h>
using i64 = long long;
struct Query {
    int l, r, id;
    Query(int l, int r, int id) :
        l{l}, r{r}, id{id} {}

    friend bool operator<(Query a, Query b) {
        int first = a.l / B;
        if (b.l / B == first) {
            if (first % 2 == 0) {
                return a.r < b.r;
            } else {
                return a.r > b.r;
            }
        }
        return first < b.l / B;
    }

    static void rangeScale(int n) {
        B = sqrt(n * 2) + 1;
    }
private:
    static int B;
};
int Query::B = 500;

