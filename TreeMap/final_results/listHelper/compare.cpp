#include "baseline.hpp"
#include "candidate.hpp"
#include "direct.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

using Clock = std::chrono::steady_clock;

void require(bool condition) {
    if (!condition)
        throw std::runtime_error("comparison mismatch");
}

template<class Key, class Compare>
void runSort(std::vector<Key>& a, Compare compare, int method) {
    if (method == 0) {
        _treemap::sortKeys(a, compare);
    } else if (method == 1) {
        a = std::move(a) | seq::sorted(compare);
    } else if (method == 4) {
        std::sort(a.begin(), a.end(), compare);
    } else {
        if (method == 2)
            a = std::move(a) | seq::sorted();
        else
            a = a | seq::sorted();
        if constexpr (std::is_same_v<Compare, std::greater<Key>>)
            std::reverse(a.begin(), a.end());
    }
}

template<class Map, class Key, class Compare>
double runConstructor(const std::vector<Key>& input,
                      const std::vector<Key>& expected, Compare compare) {
    std::atomic_signal_fence(std::memory_order_seq_cst);
    auto start = Clock::now();
    Map map(input, compare);
    auto stop = Clock::now();
    std::atomic_signal_fence(std::memory_order_seq_cst);
    require(map.empty());
    std::vector<Key> selected;
    for (std::size_t i = 0; i < expected.size(); i += std::max(std::size_t{1}, expected.size() / 4096))
        selected.push_back(expected[i]);
    for (Key key : selected)
        require(map.insert(key, false));
    require(map.size() == static_cast<int>(selected.size()));
    for (int i = 0; i < map.size(); ++i) {
        require(map.keyAt(i) == selected[i]);
        require(map.rankOf(selected[i]) == i);
        require(map(selected[i]).has_value() && !*map(selected[i]));
    }
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

template<class Key, class Compare>
void sweep(const char* type, int n, int kind, Compare compare, const char* direction) {
    std::mt19937_64 random(2026093015 + kind + n);
    std::vector<Key> input(n);
    for (int i = 0; i < n; ++i) {
        if (kind == 0)
            input[i] = static_cast<Key>(random() & ((1u << 22) - 1));
        else if (kind == 1)
            input[i] = static_cast<Key>(random());
        else if (kind == 2)
            input[i] = static_cast<Key>(random() & 15);
        else
            input[i] = kind == 3 ? i : n - i;
    }
    auto expected = input;
    std::sort(expected.begin(), expected.end(), compare);
    auto distinct = expected;
    distinct.erase(std::unique(distinct.begin(), distinct.end()), distinct.end());
    const char* distributions[] = {"range22", "fullWidth", "fewValues", "ascending", "descending"};
    const char* methods[] = {"current", "pipeTypedMove", "pipeAdaptedMove", "pipeAdaptedCopy", "stdSort"};
    std::array<int, 5> order{0, 1, 2, 3, 4};
    for (int round = -1; round < 5; ++round) {
        std::shuffle(order.begin(), order.end(), random);
        for (int method : order) {
            auto a = input; // Input preparation is outside the sort-only timer.
            std::atomic_signal_fence(std::memory_order_seq_cst);
            auto start = Clock::now();
            runSort(a, compare, method);
            auto stop = Clock::now();
            std::atomic_signal_fence(std::memory_order_seq_cst);
            require(a == expected); // Complete output, including all duplicates.
            if (round >= 0)
                std::cout << "sort," << type << ',' << n << ',' << distributions[kind] << ','
                          << direction << ',' << methods[method] << ',' << round + 1 << ','
                          << std::chrono::duration<double, std::milli>(stop - start).count() << '\n';
        }
    }
    std::array<int, 3> constructorOrder{0, 1, 2};
    const char* constructors[] = {"current", "pipeTypedMove", "pipeAdaptedMove"};
    for (int round = -1; round < 5; ++round) {
        std::shuffle(constructorOrder.begin(), constructorOrder.end(), random);
        for (int method : constructorOrder) {
            double ms;
            if (method == 0)
                ms = runConstructor<TreeMapOff<Key, bool, Compare>>(input, distinct, compare);
            else if (method == 1)
                ms = runConstructor<direct::TreeMapOff<Key, bool, Compare>>(input, distinct, compare);
            else
                ms = runConstructor<candidate::TreeMapOff<Key, bool, Compare>>(input, distinct, compare);
            if (round >= 0)
                std::cout << "constructor," << type << ',' << n << ',' << distributions[kind] << ','
                          << direction << ',' << constructors[method] << ',' << round + 1 << ',' << ms << '\n';
        }
    }
    std::cout.flush();
    std::cerr << type << ' ' << n << ' ' << distributions[kind] << ' ' << direction << " verified\n";
}

int main() {
    std::cout << "work,type,n,distribution,direction,method,round,ms\n"
              << std::fixed << std::setprecision(6);
    for (int kind = 0; kind < 5; ++kind)
        sweep<int>("int", 1000000, kind, std::less<int>{}, "ascending");
    sweep<int>("int", 2000000, 0, std::less<int>{}, "ascending");
    sweep<int>("int", 1000000, 0, std::greater<int>{}, "descending");
    sweep<long long>("longLong", 1000000, 1, std::less<long long>{}, "ascending");
}
