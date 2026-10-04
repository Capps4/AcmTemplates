#include "../../treeMap.hpp"
#define treemap baseMap
#include "before.hpp"
#undef treemap
#define treemap outlinedMap
#include "outlined.hpp"
#undef treemap
#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
using Clock = std::chrono::steady_clock;
struct Timing { double construct, insert; };
void require(bool ok) { if (!ok) throw std::runtime_error("build stage verification failed"); }
template<class Map, bool Offline>
Timing measureStages(const std::vector<int>& keys, const std::vector<int>& universe) {
    std::optional<Map> map;
    auto start = Clock::now();
    if constexpr (Offline) map.emplace(universe);
    else map.emplace(static_cast<int>(keys.size()));
    auto allocated = Clock::now();
    require(map->empty());
    std::uint64_t inserted = 0;
    auto insertStart = Clock::now();
    for (int key : keys) inserted += map->insert(key, bool((key / 2) & 1));
    auto stop = Clock::now();
    require(inserted == keys.size() && map->size() == static_cast<int>(keys.size()));
    int i = 0;
    for (auto [key, value] : *map) {
        require(key == 2*i && bool(value) == bool(i&1));
        ++i;
    }
    require(i == map->size());
    return {std::chrono::duration<double, std::milli>(allocated-start).count(),
            std::chrono::duration<double, std::milli>(stop-insertStart).count()};
}
int main() {
    constexpr int n=1000000;
    std::vector<int> keys(n), universe(2*n);
    for(int i=0;i<n;++i) keys[i]=2*i;
    for(int i=0;i<2*n;++i) universe[i]=2*i;
    std::mt19937_64 rng(20260929);
    std::shuffle(keys.begin(),keys.end(),rng);
    std::shuffle(universe.begin(),universe.end(),rng);
    using Run=Timing (*)(const std::vector<int>&,const std::vector<int>&);
    std::array<Run,4> runs{
        measureStages<baseMap::TreeMap<int,bool>,false>,
        measureStages<outlinedMap::TreeMap<int,bool>,false>,
        measureStages<baseMap::TreeMapOff<int,bool>,true>,
        measureStages<TreeMapOff<int,bool>,true>};
    const char* names[]={"sbtBase","sbtNoInline","offBase","offRadix"};
    std::array<int,4> order{0,1,2,3};
    std::cout << "candidate,round,construct_ms,insert_ms\n" << std::fixed << std::setprecision(3);
    for(int round=-1;round<7;++round) {
        std::shuffle(order.begin(),order.end(),rng);
        for(int c:order) {
            auto t=runs[c](keys,universe);
            if(round>=0) std::cout << names[c] << ',' << round+1 << ',' << t.construct << ',' << t.insert << '\n';
        }
        std::cout.flush();
    }
    std::cerr << "PASS: 4 variants x (1 warmup + 7 measured rounds), full key/value states verified\n";
}
