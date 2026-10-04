#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
int main() {
    std::mt19937 rng(20261001);
    for (int alphabet : {1, 2, 26}) {
        std::string text(1000000, 'a');
        for (char& c : text) c += rng() % alphabet;
        auto sum = [](const auto& a) { return std::accumulate(a.begin(), a.end(), std::uint64_t{}); };
        std::string label = "full-alphabet-" + std::to_string(alphabet);
        compare(label.c_str(), [&] { return sum(Legacy::zFunction(text)); },
                              [&] { return sum(zFunction(text)); });
        label = "slice-alphabet-" + std::to_string(alphabet);
        compare(label.c_str(), [&] { return sum(Legacy::zFunction(text.substr(100, 900000))); },
                              [&] { return sum(zFunction(std::string_view(text).substr(100, 900000))); });
    }
}
