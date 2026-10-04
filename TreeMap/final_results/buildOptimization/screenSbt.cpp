#define main originalBenchmarkMain
#include "../../benchmark.cpp"
#undef main
#define treemap baseMap
#include "base.hpp"
#undef treemap
#define treemap outlinedMap
#include "outlined.hpp"
#undef treemap
#define treemap resultMap
#include "result.hpp"
#undef treemap
#define treemap combinedMap
#include "combined.hpp"
#undef treemap

int main() {
    Options options;
    const Data data = makeData(options);
    const Expected expected = makeExpected(options, data);
    using Run = Result (*)(Work, const Options&, const Data&, const Expected&);
    const std::array<Run, 4> runs{
        measure<Custom<baseMap::TreeMap<int, bool>, false>>,
        measure<Custom<outlinedMap::TreeMap<int, bool>, false>>,
        measure<Custom<resultMap::TreeMap<int, bool>, false>>,
        measure<Custom<combinedMap::TreeMap<int, bool>, false>>};
    const char* names[] = {"base", "outlined", "result", "combined"};
    std::mt19937_64 orderRng(193);
    std::array<int,4> order{0,1,2,3};
    std::cout << "workload,candidate,round,ns_per_op\n" << std::fixed << std::setprecision(3);
    for (Work work : {Work::build, Work::updateMixed, Work::containsHit}) {
        for (int round=-1; round<5; ++round) {
            std::shuffle(order.begin(),order.end(),orderRng);
            for (int c : order) {
                auto r=runs[c](work,options,data,expected);
                std::cerr << workNames[static_cast<int>(work)] << ' ' << names[c] << ' ' << round+1 << ' ' << r.ns << " verified\n";
                if(round>=0) std::cout << workNames[static_cast<int>(work)] << ',' << names[c] << ',' << round+1 << ',' << r.ns << '\n';
            }
            std::cout.flush();
        }
    }
}
