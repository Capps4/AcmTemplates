#include <bits/stdc++.h>
#include "/Users/bytedance/acm_compete/cpp17/FenwickTree/Final.hpp"
#include "/Users/bytedance/acm_compete/cpp17/BenchmarkSupport.hpp"
namespace Legacy {
#include "/Users/bytedance/acm_compete/cpp17/FenwickTree/Original.hpp"
}
namespace Before {
template<class T, class Cmp = std::greater<T>>
struct Max {
    Cmp cmp;
    explicit Max(Cmp cmp = {}) : cmp(std::move(cmp)) {}
    constexpr T operator()(const T& a, const T& b) const { return cmp(b, a) ? b : a; }
};

// Commutative associative merge with a two-sided identity.
template<class T, class Merge = std::plus<T>>
class Fenwick {
    const int n;
    int firstStep = 1;
    std::vector<T> tree;
    Merge merge;
    const T identity;
public:
    explicit Fenwick(int n, Merge merge = {}, T identity = {})
        : n(n), tree(std::size_t(n) + 1, identity), merge(std::move(merge)), identity(std::move(identity)) {
        assert(n >= 0);
        while (firstStep <= n / 2) firstStep *= 2;
    }
    explicit Fenwick(const std::vector<T>& a, Merge merge = {}, T identity = {})
        : Fenwick(int(a.size()), std::move(merge), std::move(identity)) {
        for (unsigned i = 1; i <= unsigned(n); ++i) {
            tree[i] = this->merge(tree[i], a[i - 1]);
            unsigned parent = i + (i & -i);
            if (parent <= unsigned(n)) tree[parent] = this->merge(tree[parent], tree[i]);
        }
    }
    int size() const { return n; }
    void modify(int i, const T& value) {
        assert(0 <= i && i < n);
        for (unsigned p = unsigned(i) + 1; p <= unsigned(n); p += p & -p)
            tree[p] = merge(tree[p], value);
    }
    // [0,r), including r==0.
    T prefixQuery(int r) const {
        assert(0 <= r && r <= n);
        T result = identity;
        for (; r; r -= r & -r) result = merge(result, tree[r]);
        return result;
    }
    // Compatibility: inclusive prefix ending at i; i==-1 is empty.
    T posQuery(int i) const { return prefixQuery(i + 1); }
    T rangeQuery(int l, int r) const {
        static_assert(std::is_same_v<Merge, std::plus<T>> || std::is_same_v<Merge, std::plus<>>,
                      "rangeQuery requires additive merge");
        assert(0 <= l && l <= r && r <= n);
        return prefixQuery(r) - prefixQuery(l);
    }
    // Largest r such that merge([0,r))<=limit, or 0 if none. Prefixes must be monotone.
    int maxPrefix(const T& limit) const {
        int r = 0;
        T current = identity;
        for (int step = firstStep; step; step /= 2) {
            if (step > n - r) continue;
            T next = merge(current, tree[r + step]);
            if (next <= limit) { r += step; current = std::move(next); }
        }
        return r;
    }
    // Compatibility: inclusive endpoint rather than prefix length; empty returns -1.
    int select(const T& limit) const { return maxPrefix(limit) - 1; }
};

}
#undef call
int main() {
    constexpr int n = 200000;
    std::vector<long long> values(n, 1);
    std::mt19937 rng(20261001);
    std::vector<int> indices(n), limits(n);
    for (int i = 0; i < n; ++i) { indices[i] = rng() % n; limits[i] = rng() % n; }
    compare("linear-build-200K-x20", [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20; ++i) { Before::Fenwick<long long> tree(values); sum += tree.posQuery(n - 1); }
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 20; ++i) { Fenwick tree(values); sum += tree.posQuery(n - 1); }
        return sum;
    });
    auto operations = [&](auto& tree) {
        std::uint64_t sum = 0;
        for (int i : indices) { tree.modify(i, 1); sum += tree.posQuery(i); }
        return sum;
    };
    compare("modify-query-200K", [&] { Before::Fenwick<long long> tree(values); return operations(tree); },
                                  [&] { Fenwick tree(values); return operations(tree); });
    Before::Fenwick<long long> before(values);
    Fenwick after(values);
    compare("select-200K", [&] { std::uint64_t sum = 0; for (int k : limits) sum += before.select(k); return sum; },
                           [&] { std::uint64_t sum = 0; for (int k : limits) sum += after.select(k); return sum; });
}
