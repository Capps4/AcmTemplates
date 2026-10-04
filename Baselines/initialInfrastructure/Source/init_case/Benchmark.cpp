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
template <class Entry>
std::uint64_t entries(Entry entry) {
    std::string data;
    for (int i = 0; i < 100000; ++i) data += "3\n";
    std::istringstream input(data);
    auto old = std::cin.rdbuf(input.rdbuf()); std::cin.clear();
    std::uint64_t sum = 0;
    for (int i = 0; i < 100000; ++i) sum += entry() + 1;
    if (std::cin.fail()) std::exit(1);
    std::cin.rdbuf(old); std::cin.clear();
    return sum;
}
int main() {
    std::ios::sync_with_stdio(false);
    compare("case-header-100K", [] { return entries(Legacy::legacyMain); }, [] { return entries(finalMain); });
}
