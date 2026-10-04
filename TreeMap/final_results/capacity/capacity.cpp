#define main fiveWayBenchmarkMain
#include "benchmarkCore.cpp"
#undef main
#include "fixedCandidate.hpp"

template<class Map, int Mode>
struct CapacityMap {
    static constexpr bool ranked = true, mapped = true;
    Map map;
    static Map make(const Data& data) {
        if constexpr (Mode == 0) {
            return Map(static_cast<int>(data.initial.size()));
        } else if constexpr (Mode == 1) {
            return Map();
        } else {
            Map map;
            map.reserve(static_cast<int>(data.initial.size()));
            return map;
        }
    }
    explicit CapacityMap(const Data& data) : map(make(data)) {}
    bool insert(int key) { return map.insert(key, mappedValue(key)); }
    bool erase(int key) { return map.erase(key); }
    bool contains(int key) const { return map.contains(key); }
    std::size_t size() const { return map.size(); }
    int rankOf(int key) const { return map.rankOf(key); }
    int keyAt(int order) const { return map.keyAt(order); }
    template<class F> void each(F&& function) {
        for (auto [key, value] : map)
            function(key, bool(value));
    }
};

int main() {
    Options options;
    options.rounds = 7;
    const Data data = makeData(options);
    const Expected expected = makeExpected(options, data);
    std::mt19937_64 rng(options.seed ^ 172901);
    const char* names[]{"fixed", "automatic", "reserved", "fixedOnly"};
    std::cout << "workload,mode,round,n,q,ns_per_op,result_hash,final_hash,final_size\n"
              << std::fixed << std::setprecision(3);
    for (Work work : {Work::build, Work::containsHit, Work::updateMixed}) {
        for (int round = -1; round < options.rounds; ++round) {
            std::array<int, 4> order{0, 1, 2, 3};
            std::shuffle(order.begin(), order.end(), rng);
            for (int c : order) {
                Result result;
                switch (c) {
                    case 0:
                        result = measure<CapacityMap<TreeMap<int, bool>, 0>>(work, options, data, expected);
                        break;
                    case 1:
                        result = measure<CapacityMap<TreeMap<int, bool>, 1>>(work, options, data, expected);
                        break;
                    case 2:
                        result = measure<CapacityMap<TreeMap<int, bool>, 2>>(work, options, data, expected);
                        break;
                    default:
                        result = measure<CapacityMap<_treemapFixed::TreeMap<int, bool>, 0>>(work, options, data, expected);
                }
                std::cerr << workNames[static_cast<int>(work)] << ' ' << round + 1 << ' ' << names[c] << " verified\n";
                if (round >= 0)
                    std::cout << workNames[static_cast<int>(work)] << ',' << names[c] << ',' << round + 1
                              << ',' << options.n << ',' << options.q << ',' << result.ns
                              << ',' << result.flow << ',' << result.finalHash << ',' << result.finalSize << '\n';
            }
            std::cout.flush();
        }
    }
}
