#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
struct Operation { bool insert; int value; };
int main() {
    std::mt19937 rng(20261001);
    std::vector<int> live;
    std::vector<Operation> operations;
    for (int i = 0; i < 100000; ++i) {
        if (live.empty() || rng() % 100 < 55) {
            int value = rng() % 100000; live.push_back(value); operations.push_back({true, value});
        } else {
            int index = rng() % live.size(); operations.push_back({false, live[index]});
            live[index] = live.back(); live.pop_back();
        }
    }
    auto work = [&](auto& heap) {
        std::uint64_t sum = 0;
        for (auto [insert, value] : operations) {
            if (insert) heap.push(value); else heap.erase(value);
            if (heap.size()) sum += heap.top();
        }
        return sum;
    };
    compare("push-erase-top-100K", [&] { Legacy::DeletableHeap<int> heap; return work(heap); },
                                [&] { DeletableHeap<int> heap; return work(heap); });
    std::vector<std::string> values(30000);
    for (auto& value : values) value = std::to_string(rng()) + std::string(256, 'x');
    compare("bulk-string-build-30K", [&] {
        Legacy::DeletableHeap<std::string> heap;
        for (const auto& value : values) heap.push(value);
        return std::uint64_t(heap.size()) + heap.top().size();
    }, [&] {
        DeletableHeap heap(values);
        return std::uint64_t(heap.size()) + heap.top().size();
    });
}
