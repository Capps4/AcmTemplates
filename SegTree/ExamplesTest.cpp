#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include "../TestSupport.hpp"

#define Info SegExampleInfo
#define Tag SegExampleTag
#include "Final.hpp"
#undef Tag
#undef Info

#define Info TrieExampleInfo
#include "../Trie/Final.hpp"
#undef Info

#define Info SparseExampleInfo
#include "../SparseSegTree/Final.hpp"
#undef Info

int main() {
    SegTree<SegExampleInfo, SegExampleTag> sums(std::vector<int>{1, 2, 3});
    sums.modify(0, 2, SegExampleTag{4});
    CHECK(sums.query(0, 3).val == 14 && sums.query(0, 3).len == 3);
    CHECK(sums.query(1, 3).val == 9);
    sums.modify(1, SegExampleInfo(-1));
    CHECK(sums.query(0, 3).val == 7);
    SegTree<SegExampleInfo> plain(std::vector<int>{1, 2, 3});
    plain.modify(1, SegExampleInfo(9));
    CHECK(plain.query(0, 3).val == 13);
    auto pred = [](const SegExampleInfo &info) { return info.len > 0; };
    if (auto p = plain.first(0, 3, pred)) {
        CHECK(*p == 0);
    } else {
        CHECK(false);
    }
    CHECK(plain.last(0, 3, pred) == 2);
    CHECK(!plain.first(1, 1, pred));
    CHECK(!plain.last(0, 3, [](const SegExampleInfo &) { return false; }));

    using Words = StringTrie<TrieExampleInfo>;
    Words::clearInit();
    Words words;
    auto insert = [&](std::string_view s) {
        words.modify(s, [&](TrieExampleInfo &info, int dep) {
            ++info.count;
            if (dep == int(s.size())) ++info.end;
        });
    };
    insert("apple");
    auto old = words;
    insert("app");
    CHECK(words.query("app").count == 2 && words.query("app").end == 1);
    CHECK(old.query("app").count == 1 && old.query("app").end == 0);
    CHECK(words.query("bat").count == 0);

    using Bits = BinaryTrie<TrieExampleInfo, unsigned, 8>;
    Bits::clearInit();
    Bits bits;
    bits.modify(7, [](TrieExampleInfo &info, int dep) {
        ++info.count;
        if (dep == 8) ++info.end;
    });
    CHECK(bits.query(7).count == 1 && bits.query(7).end == 1);
    CHECK(bits.query(8).count == 0);

    using Sparse = SparseSegTree<SparseExampleInfo, long long>;
    Sparse::clearInit(-8, 8);
    Sparse a;
    a.modify(-1, SparseExampleInfo{1, -1});
    Sparse b = a;
    b.modify(4, SparseExampleInfo{1, 4});
    auto diff = b - a;
    CHECK(a.query(-8, 8).count == 1 && b.query(-8, 8).sum == 3);
    CHECK(diff.query(-8, 8).count == 1 && diff.query(-8, 8).sum == 4);
    CHECK(diff.first(-8, 8, [](const SparseExampleInfo &info) { return info.count >= 1; }) == 4);
    std::cout << "SegTree, string/binary trie and sparse tree footer examples passed\n";
}
