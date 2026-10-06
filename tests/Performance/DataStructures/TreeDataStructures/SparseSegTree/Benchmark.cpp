#include "../../../../../src/DataStructures/TreeDataStructures/SparseSegTree/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
struct AddedInfo {
    long long count = 0, sum = 0;
    AddedInfo operator+(const AddedInfo &b) const {
        return {count + b.count, sum + b.sum};
    }
    AddedInfo operator-(const AddedInfo &b) const {
        return {count - b.count, sum - b.sum};
    }
};
using AddedTree = SparseSegTree<AddedInfo, long long>;
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n);
    for (auto &x : a)
        x = input.shape ? 7 : int(random() % 1000000000);
    return measure(input, "sparse point insert and prefix queries; random / shared path",
                   [&]() -> std::uint64_t {
                       AddedTree::clearInit(0, 1000000001);
                       AddedTree d;
                       for (auto x : a)
                           d.modify(x, [x](AddedInfo v) {
                               ++v.count;
                               v.sum += x;
                               return v;
                           });
                       std::uint64_t sum = 0;
                       for (auto x : a)
                           sum += d.query(0, x + 1).sum;
                       return sum;
                   });
}
