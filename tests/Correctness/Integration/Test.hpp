#include "../../Support/TestSupport.hpp"
#include <memory>
#include <deque>
int main() {
    NttPoly exact{1, 2, 3}, exactRight{4, 5};
    FftPoly approximate{1, 2, 3}, realRight{4, 5};
    auto exactResult = exact * exactRight;
    auto realResult = approximate * realRight;
    std::vector<int> expected{4, 13, 22, 15};
    CHECK(exactResult.size() == expected.size() && realResult.size() == expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
        CHECK(exactResult[i].val() == expected[i]);
        CHECK(std::llround(realResult[i]) == expected[i]);
    }
    std::vector<int> values{9, 1, 9, 4};
    auto compressed = values | discreteFrom(values | sorted() | unique());
    CHECK(compressed == std::vector<int>({2, 0, 2, 1}));
    std::vector<std::vector<int>> graph{{1}, {0, 2}, {}};
    SCC scc(graph);
    CHECK(topSort(scc.g).size() == std::size_t(scc.cntBlock));
    TwoSat sat(2); sat.assign(0, true); sat.add(0, true, 1, true);
    CHECK(sat.work() && sat.ans[0] && sat.ans[1]);
    std::istringstream input("7 9");
    std::vector<int> data(2);
    seq::read(input, data);
    std::ostringstream output;
    seq::write(output, data, ","); output << "!";
    CHECK(output.str() == "7,9!");
    std::vector<std::vector<int>> undirected{{1}, {0, 2}, {1}};
    EdgeBC edgeBlocks(undirected);
    VertexBC vertexBlocks(undirected);
    RingTree ringForest(undirected);
    CHECK(edgeBlocks.cntBlock == 3 && edgeBlocks.cutDeg == std::vector<int>({1, 2, 1}));
    CHECK(vertexBlocks.csqt[1].size() == 2 && vertexBlocks.csqt.size() == 5);
    CHECK(ringForest.rings.empty());
    Flow<long long> flow(3); flow.add(0, 1, 3); flow.add(1, 2, 5);
    CHECK(flow.work(0, 2, 1) == 1 && flow.work(0, 2) == 2);
    MatrixUtil<Z> inverse(std::vector<std::vector<Z>>{{1, 2}, {3, 5}});
    CHECK(inverse.status == "OK" && inverse.inv[0][0] == Z(-5) && inverse.inv[1][1] == Z(-1));
    FullTree<int> full(undirected);
    CHECK(full.getLca(1, 2) == 1 && full.kthAncestor(2, 2) == 0);
    auto road = full.getRoad(2, 0);
    std::vector<int> walk;
    for (auto [l, r] : road) {
        int step = l <= r ? 1 : -1;
        for (int i = l;; i += step) { walk.push_back(full.idfn[i]); if (i == r) break; }
    }
    CHECK(walk == std::vector<int>({2, 1, 0}));
    CostFlow<int, long long> costFlow(3);
    costFlow.add(0, 1, 3, -2); costFlow.add(1, 2, 3, 1);
    CHECK(costFlow.work(0, 2, 1) == std::pair<int, long long>(1, -1));
    CHECK(costFlow.work(0, 2) == std::pair<int, long long>(2, -2));
    _hashmap::Impl<std::unique_ptr<int>, 17, 20> map;
    map[9] = std::make_unique<int>(7);
    ++*map[9];
    CHECK(*map[9] == 8);
    // The current Yuque template rounds ties toward +infinity, with EPS tolerance.
    CHECK(FloatPointNumber<double>(-1.5).round<int>() == -1);
    CHECK(FloatPointNumber<double>(-1.5001).round<int>() == -2);
    CHECK(FloatPointNumber<double>(0.5).round<int>() == 1);
    FILE* file = std::tmpfile(); CHECK(file);
    {
        auto writer = std::make_unique<Qoutput>(file);
        for (auto x : data) *writer << x << ',';
        *writer << "!";
    }
    std::rewind(file);
    char buffer[64]{};
    auto length = std::fread(buffer, 1, sizeof(buffer), file);
    CHECK(std::string(buffer, length) == "7,9,!");
    std::fclose(file);
    std::cout << "Template headers coexist; convolution oracle + compression + graph + stream integration PASS\n";
    return 0;
}
