#include "treeMap.hpp"
#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>

using Clock = std::chrono::steady_clock;
struct Times { double total, construct, insert; };

Times run(int mode, const std::vector<int>& keys) {
    auto start = Clock::now();
    std::optional<TreeMap<int, bool>> map;
    if (mode == 0) {
        map.emplace(static_cast<int>(keys.size()));
    } else {
        map.emplace();
        if (mode == 2)
            map->reserve(static_cast<int>(keys.size()));
    }
    auto ready = Clock::now();
    std::size_t added = 0;
    for (int key : keys)
        added += map->insert(key, bool(key & 1));
    auto stop = Clock::now();
    if (added != keys.size() || map->size() != static_cast<int>(keys.size()))
        throw std::runtime_error("insert/size mismatch");
    int next = 0;
    for (auto [key, value] : *map) {
        if (key != next++ || bool(value) != bool(key & 1))
            throw std::runtime_error("complete final key/value mismatch");
    }
    if (next != static_cast<int>(keys.size()))
        throw std::runtime_error("traversal count mismatch");
    const double scale = 1e6;
    return {std::chrono::duration<double, std::nano>(stop - start).count() / scale,
            std::chrono::duration<double, std::nano>(ready - start).count() / scale,
            std::chrono::duration<double, std::nano>(stop - ready).count() / scale};
}

int main() {
    std::mt19937_64 rng(20260929);
    std::cout << "distribution,mode,round,n,total_ms,construct_ms,insert_ms\n"
              << std::fixed << std::setprecision(6);
    const char* names[]{"fixed", "automatic", "reserved"};
    for (bool randomOrder : {true, false}) {
        std::vector<int> keys(1000000);
        std::iota(keys.begin(), keys.end(), 0);
        if (randomOrder)
            std::shuffle(keys.begin(), keys.end(), rng);
        for (int round = -1; round < 7; ++round) {
            std::array<int, 3> order{0, 1, 2};
            std::shuffle(order.begin(), order.end(), rng);
            for (int mode : order) {
                Times t = run(mode, keys);
                std::cerr << (randomOrder ? "random" : "ascending") << ' ' << round + 1
                          << ' ' << names[mode] << " verified\n";
                if (round >= 0)
                    std::cout << (randomOrder ? "random" : "ascending") << ',' << names[mode]
                              << ',' << round + 1 << ',' << keys.size() << ',' << t.total
                              << ',' << t.construct << ',' << t.insert << '\n';
            }
            std::cout.flush();
        }
    }
}
