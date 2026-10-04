#pragma once
#include <algorithm>
#include <string_view>
#include <vector>

// SNIPPET BEGIN
inline std::vector<int> kmp(std::string_view s) {
    int n = s.size();
    std::vector<int> link(n);
    for (int i = 1, j = 0; i < n; i++) {
        while (j and s[i] != s[j]) {
            j = link[j - 1];
        }
        j += (s[i] == s[j]);
        link[i] = j;
    }
    return link;
}
