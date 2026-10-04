#pragma once

// SNIPPET BEGIN
#if __has_include(<bits/stdc++.h>)
#include <bits/stdc++.h>
#else
#include <iostream>
#endif

using i64 = long long;

void solve() {
    // SNIPPET CURSOR
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int cases = 0;
    if (!(std::cin >> cases) || cases <= 0) return 0;
    while (cases--)
        solve();

    return 0;
}
