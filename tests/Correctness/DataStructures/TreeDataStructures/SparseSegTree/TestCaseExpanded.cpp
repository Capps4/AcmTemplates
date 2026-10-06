#include "../../../../../src/DataStructures/TreeDataStructures/SparseSegTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
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
void verifyAdded(const std::vector<int> &xs) {
    AddedTree::clearInit(-32, 33);
    AddedTree d;
    std::vector<int> a;
    auto verify = [&](auto &&tree, const std::vector<int> &v) {
        for (int l = -32; l <= 32; ++l)
            for (int r = l + 1; r <= 33; ++r) {
                long long cnt = 0, sum = 0;
                for (int x : v)
                    if (l <= x and x < r) {
                        ++cnt;
                        sum += x;
                    }
                auto got = tree.query(l, r);
                CHECK(got.count == cnt and got.sum == sum);
            }
    };
    for (int x : xs) {
        auto old = d;
        auto snapshot = a;
        auto view = d.view();
        d.modify(x, [x](AddedInfo v) {
            ++v.count;
            v.sum += x;
            return v;
        });
        a.push_back(x);
        verify(d, a);
        verify(old, snapshot);
        verify(view, snapshot);
        auto diff = d - old;
        verify(diff, std::vector<int>{x});
    }
    auto copy = d;
    copy.merge(d);
    auto twice = a;
    twice.insert(twice.end(), a.begin(), a.end());
    verify(copy, twice);
    verify(d, a);
}

int main() {
    runCase("SparseSegTree/01-snapshots-00", [] {
        verifyAdded({});
    });
    runCase("SparseSegTree/02-snapshots-01", [] {
        verifyAdded({0});
    });
    runCase("SparseSegTree/03-snapshots-02", [] {
        verifyAdded({-32});
    });
    runCase("SparseSegTree/04-snapshots-03", [] {
        verifyAdded({32});
    });
    runCase("SparseSegTree/05-snapshots-04", [] {
        verifyAdded({-32, 32});
    });
    runCase("SparseSegTree/06-snapshots-05", [] {
        verifyAdded({1, 1, 1});
    });
    runCase("SparseSegTree/07-snapshots-06", [] {
        verifyAdded({0, -1, 1});
    });
    runCase("SparseSegTree/08-snapshots-07", [] {
        verifyAdded({-31, 0, 31});
    });
    runCase("SparseSegTree/09-snapshots-08", [] {
        verifyAdded({-4, -3, -2, -1, 0, 1, 2, 3, 4});
    });
    runCase("SparseSegTree/10-snapshots-09", [] {
        verifyAdded({7, -2, 7, 0, -2, 31});
    });
    return finishCases(10);
}
