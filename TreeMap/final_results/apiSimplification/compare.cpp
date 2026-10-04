#define main fiveWayBenchmarkMain
#include "benchmarkCore.cpp"
#undef main
#include "previousTreeMap.hpp"

int main() {
    Options options;
    options.rounds = 5;
    const Data data = makeData(options);
    const Expected expected = makeExpected(options, data);
    std::mt19937_64 rng(options.seed ^ 3917);
    std::cout << "workload,version,round,n,q,ns_per_op,result_hash,final_hash,final_size\n"
              << std::fixed << std::setprecision(3);
    for (Work work : {Work::build, Work::updateMixed}) {
        for (int round = -1; round < options.rounds; ++round) {
            std::array<int, 4> order{0, 1, 2, 3};
            std::shuffle(order.begin(), order.end(), rng);
            for (int c : order) {
                Result result;
                const char* name = nullptr;
                switch (c) {
                    case 0:
                        name = "beforeSbt";
                        result = measure<Custom<_treemapBefore::TreeMap<int, bool>, false>>(work, options, data, expected);
                        break;
                    case 1:
                        name = "afterSbt";
                        result = measure<Custom<TreeMap<int, bool>, false>>(work, options, data, expected);
                        break;
                    case 2:
                        name = "beforeOff";
                        result = measure<Custom<_treemapBefore::TreeMapOff<int, bool>, true>>(work, options, data, expected);
                        break;
                    default:
                        name = "afterOff";
                        result = measure<Custom<TreeMapOff<int, bool>, true>>(work, options, data, expected);
                }
                std::cerr << workNames[static_cast<int>(work)] << ' ' << round + 1 << ' ' << name << " verified\n";
                if (round >= 0)
                    std::cout << workNames[static_cast<int>(work)] << ',' << name << ',' << round + 1
                              << ',' << options.n << ',' << options.q << ',' << result.ns
                              << ',' << result.flow << ',' << result.finalHash << ',' << result.finalSize << '\n';
            }
            std::cout.flush();
        }
    }
}
