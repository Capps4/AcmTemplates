#pragma once
#include "../../../../Headers/Headers.hpp"

inline std::vector<int> lyndonFactorization(std::string_view s) {
    assert(s.size() < std::size_t(INT_MAX));
    int n = s.size();
    std::vector<int> pos{0};
    for (int i = 0; i < n;) {
        int j = i + 1, k = i;
        while (j < n and (unsigned char)s[k] <= (unsigned char)s[j]) {
            k = (unsigned char)s[k] < (unsigned char)s[j] ? i : k + 1;
            ++j;
        }
        int p = j - k;
        while (i <= k) {
            i += p;
            pos.push_back(i);
        }
    }
    return pos;
}
