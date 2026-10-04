#include <bits/stdc++.h>
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
using Old = Legacy::HashMapImpl<int, 131071, 1000000>;
using New = _hashmap::HashMapImpl<int, 131071, 1000000>;
using _hashmap::u64;
template<class Map>
int readValue(const Map& map, u64 key) {
    if constexpr (std::is_same_v<Map, New>)
        return map(key).value_or(0);
    else
        return map(key);
}

template <class Map>
std::uint64_t workload(const std::vector<u64>& keys, bool interleave) {
    Map a, b;
    for (std::size_t i = 0; i < keys.size(); ++i) (interleave && i % 2 ? b : a)[keys[i]] += 1;
    std::uint64_t sum = 0;
    for (auto key : keys) sum += readValue(a, key) + readValue(b, key);
    for (auto [key, value] : a) sum += key * std::uint64_t(value);
    for (auto [key, value] : b) sum += key * std::uint64_t(value);
    return sum;
}
template <class Map>
std::uint64_t clearRepeated() {
    Map map; std::uint64_t sum = 0;
    for (int batch = 0; batch < 10000; ++batch) {
        for (u64 x = 0; x < 20; ++x) map[x] = int(x + batch);
        sum += readValue(map, 7); map.clear();
    }
    return sum;
}
int main() {
    std::mt19937_64 rng(20261001);
    for (bool repeats : {false, true}) {
        std::vector<u64> keys(300000);
        for (auto& key : keys) key = repeats ? rng() % 1000 : rng();
        compare(repeats ? "repeated-keys-300K" : "random-keys-300K",
                [&] { return workload<Old>(keys, false); }, [&] { return workload<New>(keys, false); });
        compare(repeats ? "interleaved-repeated-300K" : "interleaved-random-300K",
                [&] { return workload<Old>(keys, true); }, [&] { return workload<New>(keys, true); });
    }
    compare("sparse-clear-10K", [] { return clearRepeated<Old>(); }, [] { return clearRepeated<New>(); });
}
