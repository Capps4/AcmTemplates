#include <bits/stdc++.h>
#define main legacyMain
namespace Legacy {
#include "Original.hpp"
}
#undef main
#define main finalMain
#include "Final.hpp"
#undef main
#include "../BenchmarkSupport.hpp"
int main() {
    std::ios::sync_with_stdio(false);
    compare("entry-settings-100K", [] { std::uint64_t sum = 0; for (int i = 0; i < 100000; ++i) sum += Legacy::legacyMain() + 1; return sum; },
            [] { std::uint64_t sum = 0; for (int i = 0; i < 100000; ++i) sum += finalMain() + 1; return sum; });
}
