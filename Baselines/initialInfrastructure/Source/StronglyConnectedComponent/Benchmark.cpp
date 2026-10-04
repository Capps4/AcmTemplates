#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
template<class T> std::uint64_t checksum(const T& scc) {
    std::uint64_t sum = scc.cntBlock;
    for (std::size_t i = 0; i < scc.bel.size(); ++i) sum += std::uint64_t(scc.bel[i]) * (i + 1);
    return sum;
}
int main() {
    std::mt19937 rng(131);
    for (bool sparse : {true, false}) {
        int n = sparse ? 10000 : 50000;
        std::vector<std::vector<int>> graph(n);
        for (int x = 0; x < n; ++x) {
            if (sparse && x + 1 < n) graph[x].push_back(x + 1);
            if (!sparse) for (int edge = 0; edge < 4; ++edge) graph[x].push_back(rng() % n);
        }
        compare(sparse ? "chain-10K" : "random-degree4-50K", [&] { return checksum(Legacy::SCC(graph)); },
                [&] { return checksum(Scc(graph)); });
        compare(sparse ? "labels-only-chain" : "labels-only-random", [&] { return checksum(Legacy::SCC(graph)); },
                [&] { return checksum(Scc(graph, false)); });
    }
}
